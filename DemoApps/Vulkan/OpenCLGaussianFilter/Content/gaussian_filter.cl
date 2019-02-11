/**
 * @file
 * @author  Andre Luiz Vieira da Silva <andre.silva@freescale.com>
 * @version 1.0
 *
 * @section COPYING
 *
 * Copying: See COPYING file that comes with this distribution
 *
 * @section DESCRIPTION
 *
 * Gaussian OpenCL (Vivante) Kernel
 */

__kernel void gaussian_filter	( 	__global uchar *input,
					__global uchar *output,
					int width,
					int height,
					int channels
					
				)
{
	int x = get_global_id (0);
	int y = get_global_id (1);
	int id = (y * width) + x;
	int id1 = (y * width) + x+1;
	int id2 = (y * width) + x+2;

	float sum = 0;
	float sum2 = 0;
	float sum3 = 0;

	float kernel_weights[3][3] = 	{ 
						{1.0/16.0, 2.0/16.0, 1.0/16.0},
						{2.0/16.0, 4.0/16.0, 2.0/16.0}, 
						{1.0/16.0, 2.0/16.0, 1.0/16.0} 
					};

	if ((x < 0) || (y < 0) || (x > (width -1)) || (y > (height -1)))
	{
		output[id] = input[id];
		return;
	}

	if (channels == 1)
	{
	  sum = input[id - width - 1] * kernel_weights[0][0];
	  sum += input[id - width] * kernel_weights[0][1];
	  sum += input[id - width + 1] * kernel_weights[0][2];

	  sum += input[id - 1] * kernel_weights[1][0];
	  sum += input[id] * kernel_weights[1][1];
	  sum += input[id + 1] * kernel_weights[1][2];

	  sum += input[id + width - 1] * kernel_weights[2][0];
	  sum += input[id + width] * kernel_weights[2][1];
	  sum += input[id + width + 1] * kernel_weights[2][2];

	  output[id] = sum;
	}
	
	else if (channels == 3)
	{
	 
	  // R Channel
	  sum = input[id - width - 1] * kernel_weights[0][0];
	  sum += input[id - width] * kernel_weights[0][1];
	  sum += input[id - width + 1] * kernel_weights[0][2];

	  sum += input[id - 1] * kernel_weights[1][0];
	  sum += input[id] * kernel_weights[1][1];
	  sum += input[id + 1] * kernel_weights[1][2];

	  sum += input[id + width - 1] * kernel_weights[2][0];
	  sum += input[id + width] * kernel_weights[2][1];
	  sum += input[id + width + 1] * kernel_weights[2][2];

	  // G Channel
	  sum2 = input[id1 - width - 1] * kernel_weights[0][0];
	  sum2 += input[id1 - width] * kernel_weights[0][1];
	  sum2 += input[id1 - width + 1] * kernel_weights[0][2];

	  sum2 += input[id1 - 1] * kernel_weights[1][0];
	  sum2 += input[id1] * kernel_weights[1][1];
	  sum2 += input[id1 + 1] * kernel_weights[1][2];

	  sum2 += input[id1 + width - 1] * kernel_weights[2][0];
	  sum2 += input[id1 + width] * kernel_weights[2][1];
	  sum2 += input[id1 + width + 1] * kernel_weights[2][2];

	  
	  // B Channel
	  sum3 = input[id2 - width - 1] * kernel_weights[0][0];
	  sum3 += input[id2 - width] * kernel_weights[0][1];
	  sum3 += input[id2 - width + 1] * kernel_weights[0][2];

	  sum3 += input[id2 - 1] * kernel_weights[1][0];
	  sum3 += input[id2] * kernel_weights[1][1];
	  sum3 += input[id2 + 1] * kernel_weights[1][2];

	  sum3 += input[id2 + width - 1] * kernel_weights[2][0];
	  sum3 += input[id2 + width] * kernel_weights[2][1];
	  sum3 += input[id2 + width + 1] * kernel_weights[2][2];

	  output[id] = sum;
 	  output[id1] = sum2;
	  output[id2] = sum3;
	}
	
	else
	{
	  output[id] = 0;
	  output[id1] = 255;
	  output[id2] = 255;
	}
}
