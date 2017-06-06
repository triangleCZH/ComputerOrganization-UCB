// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */

void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement)
{
  /* The two outer for loops iterate through each pixel */
    #pragma omp parallel for schedule(dynamic,1) collapse(2)
  for (int y = 0; y < imageHeight; y++)
  {  //these two loops sets the default value to 0 so we can cycle through a smaller loop
    for (int x = 0; x < imageWidth; x++)
    {
      depth[y * imageWidth + x] = 0;
    }
  }
  int y_lower_bound = featureHeight;
  int y_upper_bound = imageHeight - featureHeight;
  int x_lower_bound = featureWidth;
  int x_upper_bound = imageWidth - featureWidth;

  omp_set_dynamic(0);
  omp_set_num_threads(64);
    #pragma omp parallel for schedule(dynamic,1) collapse(2)
  for (int y = y_lower_bound; y < y_upper_bound; y++)
  {
    for (int x = x_lower_bound; x < x_upper_bound; x++)
    {
      float minimumSquaredDifference = -1;
      int minimumDy = 0;
      int minimumDx = 0;

      /* Iterate through all feature boxes that fit inside the maximum displacement box.
         centered around the current pixel. */
      for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
      {
        for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
        {
          /* Skip feature boxes that dont fit in the displacement box. */
          if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)           //fixme: narrow down the range of feature box search, beginning and ending at points that the *feature box can ALWAYS fit into the displacement box*, so we do not need to do this test in each loop
          {
            continue;
          }

          float squaredDifference = 0;
					int upp_bound = (2*(featureWidth)+1)/4*4;
					__m128 temp_sum = _mm_setzero_ps();
          __m128 tail_sum = _mm_setzero_ps();
					float sum_array[4];
          /* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
          for (int boxY = -featureHeight; boxY <= featureHeight; boxY++)
          {
            int leftY = y + boxY;
            int rightY = y + dy + boxY;


            // now these two fucking somethings, to calculate these fucking long expressions and save them for future use
            //because these two damn long expressions are fucking slow
            //fuck
            int* something = left+leftY*imageWidth+x-featureWidth;
            int* something2 = right+rightY*imageWidth+dx+x-featureWidth;

						for(int i = 0; i < upp_bound; i+=4){
							__m128 temp0 = _mm_loadu_ps((__m128*)(something + i));
							__m128 temp1 = _mm_loadu_ps((__m128*)(something2 + i));
							temp0 = _mm_sub_ps(temp0, temp1);
							temp0 = _mm_mul_ps(temp0, temp0);
							temp_sum = _mm_add_ps(temp_sum, temp0);
						}
            __m128 temp0 = _mm_loadu_ps((__m128*)(something + upp_bound));
            __m128 temp1 = _mm_loadu_ps((__m128*)(something2 + upp_bound));
            temp0 = _mm_sub_ps(temp0, temp1);
            temp0 = _mm_mul_ps(temp0, temp0);
            tail_sum = _mm_add_ps(tail_sum, temp0);
          }
					_mm_storeu_ps(sum_array, temp_sum);
					squaredDifference += sum_array[0]+sum_array[1]+sum_array[2]+sum_array[3];// the main summing part

          //the tail summing part
          _mm_storeu_ps(sum_array, tail_sum);
          for (size_t i = 0; i < 2*featureWidth+1-upp_bound; i++) {
            squaredDifference += sum_array[i];
          }

          /*
             Check if you need to update minimum square difference.
             This is when either it has not been set yet, the current
             squared displacement is equal to the min and but the new
             displacement is less, or the current squared difference
             is less than the min square difference.
           */
          if ((minimumSquaredDifference == -1) || (minimumSquaredDifference == squaredDifference) || (minimumSquaredDifference > squaredDifference)) {
            if ((minimumSquaredDifference == -1) || ((minimumSquaredDifference == squaredDifference) && (sqrt(dx * dx + dy * dy) < sqrt(minimumDx * minimumDx + minimumDy * minimumDy))) || (minimumSquaredDifference > squaredDifference))
            {            //fixme: we should write our own displacement function instead of using displacement naive
                         //fixme: we could build nested if instead of this insane condition testing so it could jump out at the first failing condition
              minimumSquaredDifference = squaredDifference;
              minimumDx = dx;
              minimumDy = dy;
            }
          }
        }
      }

      /*
         Set the value in the depth map.
         If max displacement is equal to 0, the depth value is just 0.
       */
      if (minimumSquaredDifference != -1)
      {
        if (maximumDisplacement == 0)
        {
          depth[y * imageWidth + x] = 0;
        }
        else
        {
          depth[y * imageWidth + x] = sqrt(minimumDx * minimumDx + minimumDy * minimumDy);
        }        //fixme: memorize the displacement naive value from before instead of creating new values
      }
      else
      {
        depth[y * imageWidth + x] = 0;
      }
    }
  }
}
