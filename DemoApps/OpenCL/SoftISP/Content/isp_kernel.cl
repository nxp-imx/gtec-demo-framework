/****************************************************************************************************************************************************
* Copyright 2017 NXP
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the NXP. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/

#define src_step 1920
#define dst_step 1920
#define dst_step_dsp 1920
#define WIDTH 1920

#define R_num (1920*1080/4)
#define G_num (1920*1080/2)
#define B_num (1920*1080/4)

__kernel void badpixel( __global uchar *src, __global uchar *dst) 
{ 
	int i = get_global_id(0);
	int j = get_global_id(1);
	 
	int img_arrayA[4],img_arrayB[4],img_arrayGA[4],img_arrayGB[4];
	uchar8 lineA,lineB,lineC,lineD,lineE,lineF;
	lineA = vload8(0, src + mad24(j*2-2, src_step, i*4-2));
	lineB = vload8(0, src + mad24(j*2-1, src_step, i*4-2));
	lineC = vload8(0, src + mad24(j*2, src_step, i*4-2));
	lineD = vload8(0, src + mad24(j*2+1, src_step, i*4-2));
	lineE = vload8(0, src + mad24(j*2+2, src_step, i*4-2));
	lineF = vload8(0, src + mad24(j*2+3, src_step, i*4-2));
	 
	uchar4  pixel_center0 = (uchar4)(lineC.s2, lineC.s3, lineC.s4, lineC.s5);
	uchar4  pixel_center1 = (uchar4)(lineD.s2, lineD.s3, lineD.s4, lineD.s5);

	int temp;
	
	//B				
	img_arrayA[0] = (int)lineA.s2;								
	img_arrayA[1] = (int)lineC.s0;	
	img_arrayA[2] = (int)lineC.s4;								
	img_arrayA[3] = (int)lineE.s2;					

	#pragma unroll			
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
		{ 
			if (img_arrayA[k] > img_arrayA[k + 1])
				{
					temp = img_arrayA[k];
					img_arrayA[k] = img_arrayA[k + 1];
					img_arrayA[k + 1] = temp;
				}
			}				
	}													
	int AVE = (img_arrayA[1] + img_arrayA[2])>>1;
	int ABS = img_arrayA[3] - img_arrayA[0];
	int up_value = AVE + ABS;
	int down_value = AVE - ABS;

	if (pixel_center0.s0 > up_value || pixel_center0.s0 < down_value)
		pixel_center0.s0 = convert_uchar(AVE);

	// G 
	img_arrayGA[0] = lineB.s2;							 
	img_arrayGA[1] = lineB.s4;						 
	img_arrayGA[2] = lineD.s2;						 	
	img_arrayGA[3] = lineD.s4;

	#pragma unroll
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
			if (img_arrayGA[k] > img_arrayGA[k + 1])
			{
				temp = img_arrayGA[k];
				img_arrayGA[k] = img_arrayGA[k + 1];
				img_arrayGA[k + 1] = temp;
			}	
	}
								
	AVE = (img_arrayGA[1] + img_arrayGA[2])>>1;
	ABS = img_arrayGA[3] - img_arrayGA[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);
					
	if (pixel_center0.s1 > up_value || pixel_center0.s1 < down_value)
		pixel_center0.s1 = convert_uchar(AVE);
				
	img_arrayB[0] = (int)lineA.s4;				
	img_arrayB[1] = (int)lineC.s2;
	img_arrayB[2] = (int)lineC.s6;						
	img_arrayB[3] = (int)lineE.s4;	

	#pragma unroll			
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
		{ 
			if (img_arrayB[k] > img_arrayB[k + 1])
			{
				temp = img_arrayB[k];
				img_arrayB[k] = img_arrayB[k + 1];
				img_arrayB[k + 1] = temp;
				}
		}
						
	}													
	AVE = (img_arrayB[2] + img_arrayB[1])>>1;
	ABS = img_arrayB[3] - img_arrayB[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);

	if (pixel_center0.s2 > up_value || pixel_center0.s2 < down_value)
		pixel_center0.s2 = convert_uchar(AVE);
									
					
	img_arrayGB[0] = (int)lineB.s4;			 
	img_arrayGB[1] = (int)lineB.s6;		 
	img_arrayGB[2] = (int)lineD.s4;		 	
	img_arrayGB[3] = (int)lineD.s6;
	#pragma unroll
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
			if (img_arrayGA[k] > img_arrayGB[k + 1])
				{
				temp = img_arrayGB[k];
				img_arrayGB[k] = img_arrayGB[k + 1];
				img_arrayGB[k + 1] = temp;
			}	
	}
	AVE = (img_arrayGB[1] + img_arrayGB[2])>>1;
	ABS = img_arrayGB[3] - img_arrayGB[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);

	if (pixel_center0.s3 > up_value || pixel_center0.s3 < down_value)
		pixel_center0.s3 = convert_uchar(AVE);
			
	vstore4(pixel_center0, 0 ,dst+ mad24(j*2, dst_step, i<<2 ));

	img_arrayGA[0] = (int)lineC.s1;							 
	img_arrayGA[1] = (int)lineC.s3;					 
	img_arrayGA[2] = (int)lineE.s1;						 	
	img_arrayGA[3] = (int)lineE.s3;
	#pragma unroll
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
			if (img_arrayGA[k] > img_arrayGA[k + 1])
			{
				temp = img_arrayGA[k];
				img_arrayGA[k] = img_arrayGA[k + 1];
				img_arrayGA[k + 1] = temp;
			}	
	}
	AVE = (img_arrayGA[1] + img_arrayGA[2])>>1;
	ABS = img_arrayGA[3] - img_arrayGA[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);

	if (pixel_center1.s0 > up_value || pixel_center1.s0 < down_value)
		pixel_center1.s0 = convert_uchar(AVE);
		   
	img_arrayA[0] = (int)lineB.s3;	
	img_arrayA[1] = (int)lineD.s1;	
	img_arrayA[2] = (int)lineD.s5;	
	img_arrayA[3] = (int)lineF.s3;	
	#pragma unroll			
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
		 { 
			if (img_arrayA[k] > img_arrayA[k + 1])
			{
				temp = img_arrayA[k];
				img_arrayA[k] = img_arrayA[k + 1];
				img_arrayA[k + 1] = temp;
			}
		}						
	}													
	AVE = (img_arrayA[1] + img_arrayA[2] )>>1;
	ABS = img_arrayA[3] - img_arrayA[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);

	if (pixel_center1.s1 > up_value || pixel_center1.s1 < down_value)
		pixel_center1.s1 = convert_uchar(AVE);		
	
	img_arrayGB[0] = (int)lineC.s3;							 
	img_arrayGB[1] = (int)lineC.s5;						 
	img_arrayGB[2] = (int)lineE.s3;						 	
	img_arrayGB[3] = (int)lineE.s5;
	#pragma unroll
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
			if (img_arrayGB[k] > img_arrayGB[k + 1])
			{
				temp = img_arrayGB[k];
				img_arrayGB[k] = img_arrayGB[k + 1];
				img_arrayGB[k + 1] = temp;
			}	
	}
	AVE = (img_arrayGB[1] + img_arrayGB[2])>>1;
	ABS = img_arrayGB[3] - img_arrayGB[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);

	if (pixel_center1.s2 > up_value || pixel_center1.s2 < down_value)
		pixel_center1.s2 = convert_uchar(AVE);
			
	img_arrayB[0] = (int)lineB.s5;					
	img_arrayB[1] = (int)lineD.s3;
	img_arrayB[2] = (int)lineD.s7;					
	img_arrayB[3] = (int)lineF.s5;		
	#pragma unroll			
	for (int pass = 1; pass < 4; pass++)
	{
		for (int k = 0; k< 4 - pass; k++)
		{ 
			if (img_arrayB[k] > img_arrayB[k + 1])
			{
				temp = img_arrayB[k];
				img_arrayB[k] = img_arrayB[k + 1];
				img_arrayB[k + 1] = temp;
			}
		}
						
	}													
	AVE = (img_arrayB[1] + img_arrayB[2])>>1;
	ABS = img_arrayB[3] - img_arrayB[0];
	up_value = AVE + ABS;
	down_value = abs(AVE - ABS);

	if (pixel_center1.s3 > up_value || pixel_center1.s3 < down_value)
		pixel_center1.s3 = convert_uchar(AVE);						

	vstore4(pixel_center1 , 0 ,dst+ mad24(j*2+1, dst_step, i<<2 ));
}

__kernel void sigma(__global const uchar *src,
                  __global int *pixel_valuer, __global int *pixel_valueg,__global int *pixel_valueb)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	uchar8 pix = vload8(0, src + mad24(j*2, src_step, i*8));
	uchar8 pix2 = vload8(0, src + mad24(j*2+1, src_step, i*8));
	atomic_add(pixel_valueb,pix.s0); 
	atomic_add(pixel_valueg,pix.s1);
	atomic_add(pixel_valueb,pix.s2);
	atomic_add(pixel_valueg,pix.s3);
	atomic_add(pixel_valueb,pix.s4); 
	atomic_add(pixel_valueg,pix.s5);
	atomic_add(pixel_valueb,pix.s6);
	atomic_add(pixel_valueg,pix.s7);
	
	atomic_add(pixel_valueg,pix2.s0); 
	atomic_add(pixel_valuer,pix2.s1);
	atomic_add(pixel_valueg,pix2.s2);
	atomic_add(pixel_valuer,pix2.s3);
	atomic_add(pixel_valueg,pix2.s4); 
	atomic_add(pixel_valuer,pix2.s5);
	atomic_add(pixel_valueg,pix2.s6);
	atomic_add(pixel_valuer,pix2.s7);
}

__kernel void awb(__global const uchar *src,
                  __global int *pixel_valuer, __global int *pixel_valueg,__global int *pixel_valueb, __global uchar *dst)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	uchar8 pix = vload8(0, src + mad24(j*2, src_step, i*8));
	uchar8 pix2 = vload8(0, src + mad24(j*2+1, src_step, i*8));
	float R_AVG = (float)(*pixel_valuer)/(float)R_num;
	float G_AVG = (float)(*pixel_valueg)/(float)G_num;
	float B_AVG = (float)(*pixel_valueb)/(float)B_num;
	float alpha = G_AVG/R_AVG;
	float beta = G_AVG/B_AVG;
	
	uchar8 out = 0;
	uchar8 out2 = 0;
	
	uchar temp1 = convert_uchar_sat_rte(pix.s0*beta);
	uchar temp2 = convert_uchar_sat_rte(pix.s2*beta);
	uchar temp3 = convert_uchar_sat_rte(pix.s4*beta);
	uchar temp4 = convert_uchar_sat_rte(pix.s6*beta); 
	out = (uchar8)(temp1, pix.s1, temp2, pix.s3, temp3, pix.s5, temp4, pix.s7);	
	
	temp1 = convert_uchar_sat_rte(pix2.s1*alpha); 
	temp2 = convert_uchar_sat_rte(pix2.s3*alpha);
	temp3 = convert_uchar_sat_rte(pix2.s5*alpha); 
	temp4 = convert_uchar_sat_rte(pix2.s7*alpha);
	out2 = (uchar8)(pix2.s0, temp1, pix2.s2, temp2, pix2.s4, temp3, pix2.s6, temp4);

	vstore8(out , 0 ,dst+ mad24(j*2, dst_step, i*8 ));	
	vstore8(out2 , 0 ,dst+ mad24(j*2+1, dst_step, i*8 ));
}

__kernel void equalize1(__global const uchar *src,
                  __global int *pixel_valuer, __global int *pixel_valueg,__global int *pixel_valueb)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	uchar8 pix0 = vload8(0, src + mad24(2*j, src_step, i*8));
	uchar8 pix1 = vload8(0, src + mad24(2*j + 1, src_step, i*8));
	int8 tmp0 = convert_int8(pix0);
	atomic_inc(pixel_valueb + tmp0.s0);
	atomic_inc(pixel_valueg + tmp0.s1);
	atomic_inc(pixel_valueb + tmp0.s2);
	atomic_inc(pixel_valueg + tmp0.s3);
	atomic_inc(pixel_valueb + tmp0.s4);
	atomic_inc(pixel_valueg + tmp0.s5);
	atomic_inc(pixel_valueb + tmp0.s6);
	atomic_inc(pixel_valueg + tmp0.s7);
	
	int8 tmp1 = convert_int8(pix1);
	atomic_inc(pixel_valueg + tmp1.s0);
	atomic_inc(pixel_valuer + tmp1.s1);
	atomic_inc(pixel_valueg + tmp1.s2);
	atomic_inc(pixel_valuer + tmp1.s3);
	atomic_inc(pixel_valueg + tmp1.s4);
	atomic_inc(pixel_valuer + tmp1.s5);
	atomic_inc(pixel_valueg + tmp1.s6);
	atomic_inc(pixel_valuer + tmp1.s7);
}
__kernel void equalize2(__global int *in_R, __global int *in_G,__global int *in_B, __global int *out_R, __global int *out_G,__global int *out_B)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);
	int gid = get_group_id(1)* get_num_groups(0) + get_group_id(0);

	if (gid == 0){
		float scaler, scaleg, scaleb;
		int sumr = 0;
        int ir = 0;
        while (!(*(in_R + ir)))
            ++ir;
        if (R_num == (*(in_R + ir))){
            	scaler = 1;
            	*(out_R + ir) = ir;
        }
        else{
			scaler = 255.f / (R_num - *(in_R + ir));
			*(out_R + ir) = 0;
			ir++;
           	for (; ir < 256; ir++){
                	sumr += *(in_R + ir);
                	*(out_R + ir) = convert_int_sat_rte(convert_float(sumr) * scaler);
           	}
		}
		int sumg = 0;
        int ig = 0;
        while (!(*(in_G + ig)))
            ++ig;
        if (G_num == *(in_G + ig)){
            	scaleg = 1;
            	*(out_G + ig) = ig;
        }
        else{
			scaleg = 255.f / (G_num - *(in_G + ig));
			*(out_G + ig) = 0;
			ig++;
            for (; ig < 256; ig++){
                sumg += *(in_G + ig);
                *(out_G + ig) = convert_int_sat_rte(convert_float(sumg) * scaleg);
            }
		}
		int sumb = 0;
        int ib = 0;
        while (!(*(in_B + ib)))
            ++ib;
        if (B_num == *(in_B + ib)){
            	scaleb = 1;
               	*(out_B + ib) = ib;
        }
        else{
			scaleb = 255.f / (B_num - *(in_B + ib));
			*(out_B + ib) = 0;
			ib++;
            for (; ib < 256; ib++){ 
                sumb += *(in_B + ib);
                *(out_B + ib) = (int)convert_uchar_sat_rte(convert_float(sumb) * scaleb);
            }
		}
	}
}

__kernel void equalize3(__global const uchar *src,
                  __global int *pixel_valuer, __global int *pixel_valueg,__global int *pixel_valueb, __global uchar *dst)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);
	uchar8 pix0 = vload8(0, src + mad24(2*j, src_step, i * 8));
	uchar8 pix1 = vload8(0, src + mad24(2*j + 1, src_step, i * 8));
	int8 pixelEven, pixelOdd;
	uchar8 pixelOut0, pixelOut1;
	pixelEven = convert_int8(pix0);
	pixelOdd = convert_int8(pix1);
	pixelOut0.s0 = convert_uchar(*(pixel_valueb + pixelEven.s0));
	pixelOut0.s1 = convert_uchar(*(pixel_valueg + pixelEven.s1));
	pixelOut0.s2 = convert_uchar(*(pixel_valueb + pixelEven.s2));
	pixelOut0.s3 = convert_uchar(*(pixel_valueg + pixelEven.s3));
	pixelOut0.s4 = convert_uchar(*(pixel_valueb + pixelEven.s4));
	pixelOut0.s5 = convert_uchar(*(pixel_valueg + pixelEven.s5));
	pixelOut0.s6 = convert_uchar(*(pixel_valueb + pixelEven.s6));
	pixelOut0.s7 = convert_uchar(*(pixel_valueg + pixelEven.s7));
	
	pixelOut1.s0 = convert_uchar(*(pixel_valueg + pixelOdd.s0));
	pixelOut1.s1 = convert_uchar(*(pixel_valuer + pixelOdd.s1));
	pixelOut1.s2 = convert_uchar(*(pixel_valueg + pixelOdd.s2));
	pixelOut1.s3 = convert_uchar(*(pixel_valuer + pixelOdd.s3));
	pixelOut1.s4 = convert_uchar(*(pixel_valueg + pixelOdd.s4));
	pixelOut1.s5 = convert_uchar(*(pixel_valuer + pixelOdd.s5));
	pixelOut1.s6 = convert_uchar(*(pixel_valueg + pixelOdd.s6));
	pixelOut1.s7 = convert_uchar(*(pixel_valuer + pixelOdd.s7));
	
	vstore8(pixelOut0 , 0 ,dst + mad24(j * 2, dst_step, i * 8));
	vstore8(pixelOut1 , 0 ,dst + mad24(j * 2 + 1, dst_step, i * 8));
}
inline float4 Pattern(float4 PATTERN , float4 value, float4 temp, float4 Dvec){ 

	const float4 kA = (float4)(-1.0/8.0, -1.5/8.0,  0.5/8.0, -1.0/8.0);
	const float4 kB = (float4)( 2.0/8.0,  0.0,	  0.0,		4.0/8.0);
	const float4 kD = (float4)( 0.0,		2.0/8.0, -1.0/8.0, -1.0/8.0);

	const float4 kE = kA.xywz;
	const float4 kF = kB.xywz;

	value += temp;    
			
	PATTERN.yzw += (kD.yz * Dvec.x).xyy;						

	PATTERN += (kA.xyz * (float3)(value.x,value.x,value.x)).xyzx + (kE.xyw * (float3)(value.z,value.z,value.z)).xyxz;		
	PATTERN.xw  += kB.xw * (float2)(value.y,value.y);								
	PATTERN.xz  += kF.xz * (float2)(value.w,value.w);

	return PATTERN;

}

__kernel void debayer( __global uchar *psrc, __global uchar *pdst) 
{ 
	int i = get_global_id(0);
	int j = get_global_id(1);
	
	uchar8 lineA, lineB, lineC, lineD, lineE, lineF;

	lineA = vload8(0, psrc + mad24(j*2-2, src_step, i*4-2));
	lineB = vload8(0, psrc + mad24(j*2-1, src_step, i*4-2));
	lineC = vload8(0, psrc + mad24(j*2, src_step, i*4-2));
	lineD = vload8(0, psrc + mad24(j*2+1, src_step, i*4-2));
	lineE = vload8(0, psrc + mad24(j*2+2, src_step, i*4-2));
	lineF = vload8(0, psrc + mad24(j*2+3, src_step, i*4-2));
	 
	const float4 kC = (float4)( 4.0/8.0,  6.0/8.0,  5.0/8.0,  5.0/8.0);

	uchar16 out1, out2;
	float4 Dvec = (float4)(lineB.s1, lineD.s1, lineB.s3, lineD.s3); 

	float4 PATTERN = (kC.xyz * lineC.s2).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	float4 value = (float4)(lineA.s2, lineB.s2,  lineC.s0, lineC.s1);
	float4 temp = (float4)(lineE.s2, lineD.s2,  lineC.s4, lineC.s3);

	float4 PATTERN_One =  Pattern(PATTERN , value , temp, Dvec);	

    Dvec = (float4)(lineB.s2, lineD.s2, lineB.s4, lineD.s4); 

	PATTERN = (kC.xyz * lineC.s3).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineA.s3, lineB.s3,  lineC.s1, lineC.s2);
	temp = (float4)(lineE.s3, lineD.s3,  lineC.s5, lineC.s4);

	float4 PATTERN_Two =  Pattern(PATTERN , value , temp, Dvec);						

    Dvec = (float4)(lineB.s3, lineD.s3, lineB.s5, lineD.s5); 

	PATTERN = (kC.xyz * lineC.s4).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineA.s4, lineB.s4,  lineC.s2, lineC.s3);
	temp = (float4)(lineE.s4, lineD.s4,  lineC.s6, lineC.s5);

	float4 PATTERN_Three =  Pattern(PATTERN , value , temp, Dvec);	

    Dvec = (float4)(lineB.s4, lineD.s4, lineB.s6, lineD.s6); 

	PATTERN = (kC.xyz * lineC.s5).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineA.s5, lineB.s5,  lineC.s3, lineC.s4);
	temp = (float4)(lineE.s5, lineD.s5,  lineC.s7, lineC.s6);

	float4 PATTERN_Four =  Pattern(PATTERN , value , temp, Dvec);	
	
	out1 = (uchar16)(
		(uchar)(PATTERN_One.y > 255 ? 255 : PATTERN_One.y),(uchar)(PATTERN_One.x > 255 ? 255 : PATTERN_One.x), lineC.s2,255,	
		(uchar)(PATTERN_Two.w > 255 ? 255 : PATTERN_Two.w),lineC.s3,(uchar)(PATTERN_Two.z > 255 ? 255 : PATTERN_Two.z),255,
		(uchar)(PATTERN_Three.y > 255 ? 255 : PATTERN_Three.y),(uchar)(PATTERN_Three.x > 255 ? 255 : PATTERN_Three.x), lineC.s4,255,
		(uchar)(PATTERN_Four.w > 255 ? 255 : PATTERN_Four.w),lineC.s5,(uchar)(PATTERN_Four.z > 255 ? 255 : PATTERN_Four.z),255);

	vstore16(out1, 0, pdst + mad24(j*2, dst_step_dsp*4, i<<4));

	Dvec = (float4)(lineC.s1, lineE.s1, lineC.s3, lineE.s3); 

	PATTERN = (kC.xyz * lineD.s2).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineB.s2, lineC.s2,  lineD.s0, lineD.s1);
	temp = (float4)(lineF.s2, lineE.s2,  lineD.s4, lineD.s3);

	PATTERN_One =  Pattern(PATTERN , value , temp, Dvec);	

    Dvec = (float4)(lineC.s2, lineE.s2, lineC.s4, lineE.s4); 

	PATTERN = (kC.xyz * lineD.s3).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineB.s3, lineC.s3,  lineD.s1, lineD.s2);
	temp = (float4)(lineF.s3, lineE.s3,  lineD.s5, lineD.s4);

	PATTERN_Two =  Pattern(PATTERN , value , temp, Dvec);						

    Dvec = (float4)(lineC.s3, lineE.s3, lineC.s5, lineE.s5); 

	PATTERN = (kC.xyz * lineD.s4).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineB.s4, lineC.s4,  lineD.s2, lineD.s3);
	temp = (float4)(lineF.s4, lineE.s4,  lineD.s6, lineD.s5);

	PATTERN_Three =  Pattern(PATTERN , value , temp, Dvec);	

    Dvec = (float4)(lineC.s4, lineE.s4, lineC.s6, lineE.s6); 

	PATTERN = (kC.xyz * lineD.s5).xyzz;
	  
	Dvec.xy += Dvec.zw;                
	Dvec.x  += Dvec.y;                 

   	value = (float4)(lineB.s5, lineC.s5,  lineD.s3, lineD.s4);
	temp = (float4)(lineF.s5, lineE.s5,  lineD.s7, lineD.s6);

	PATTERN_Four =  Pattern(PATTERN , value , temp, Dvec);

	out2 = (uchar16)(
	(uchar)(PATTERN_One.z > 255 ? 255 : PATTERN_One.z),lineD.s2, (uchar)(PATTERN_One.w > 255 ? 255 : PATTERN_One.w), 255, 				 
	lineD.s3,(uchar)(PATTERN_Two.x > 255 ? 255 : PATTERN_Two.x),(uchar)(PATTERN_Two.y > 255 ? 255 : PATTERN_Two.y), 255 ,
	(uchar)(PATTERN_Three.z > 255 ? 255 : PATTERN_Three.z),lineD.s4,(uchar)(PATTERN_Three.w > 255 ? 255 : PATTERN_Three.w), 255, 				 
 	lineD.s5,(uchar)(PATTERN_Four.x > 255 ? 255 : PATTERN_Four.x),(uchar)(PATTERN_Four.y > 255 ? 255 : PATTERN_Four.y), 255);

	vstore16(out2, 0, pdst+ mad24(j*2 + 1, dst_step_dsp*4, i<<4 ));
}
__kernel void rgba2yuyv(__global const uchar * src, __global uchar * dst1, __global uchar * dst2) 
{ 
	int i = get_global_id(0);
	int j = get_global_id(1);
	int src_idx = mad24(j, WIDTH * 4, i*4);
	int y_idx = mad24(j, WIDTH, i);
	int uv_idx = mad24(j, WIDTH * 2, i*2);

	float4 outColor; 
	
	//BGRA
	outColor.x = 0.098 * src[src_idx] + 0.504 * src[src_idx+1] + 0.257 * src[src_idx+2] + 16;	
	outColor.y = 0.439 * src[src_idx] - 0.291 * src[src_idx+1] - 0.148 * src[src_idx+2] + 128;	
	outColor.z = (-0.071) * src[src_idx] - 0.368 * src[src_idx+1] + 0.439 * src[src_idx+2] + 128;
	uchar4 outColor1 = convert_uchar4(outColor);
	dst1[y_idx] = outColor1.x;
	dst2[uv_idx] = outColor1.y;
	dst2[uv_idx + 1] = outColor1.z;
}
#define SPCW (30*30)
__kernel void bilateral( __global uchar* input_image, __global uchar* output_image) 
{ 
	int x = get_global_id(0);
    int y = get_global_id(1);
    
    float med1[7][7] = {1.000, 0.722, 0.556, 0.500, 0.556, 0.722, 1.000,
						0.722, 0.444, 0.278, 0.222, 0.278, 0.444, 0.722,
						0.556, 0.278, 0.111, 0.056, 0.111, 0.278, 0.556,
						0.500, 0.222, 0.056, 0.000, 0.056, 0.222, 0.500,
						0.556, 0.278, 0.111, 0.056, 0.111, 0.278, 0.556,
						0.722, 0.444, 0.278, 0.222, 0.278, 0.444, 0.722,
						1.000, 0.722, 0.556, 0.500, 0.556, 0.722, 1.000};

    uchar m_pixel;
    uchar16 lineA, lineB, lineC, lineD;
    uchar8 tmp[7], lineP, rangeWeight0, out;
    float8 rangeWeight1 = 0, rangeWeight2 = 0, spaceWeight = 0, weight = 0, sumWeight = 0;
    float8 tmpPixel = 0, tmpOut = 0;
    
    lineA = vload16(0, input_image + (y - 3) * WIDTH + x * 8 - 3);
    lineB = vload16(0, input_image + (y - 2) * WIDTH + x * 8 - 3);
    lineC = vload16(0, input_image + (y - 1)  * WIDTH + x * 8 - 3);
    lineD = vload16(0, input_image + (y + 0)  * WIDTH + x * 8 - 3);
    
    lineP = lineD.s3456789a;
    tmp[0] = lineA.s01234567;
	tmp[1] = lineA.s12345678;
	tmp[2] = lineA.s23456789;
	tmp[3] = lineA.s3456789a;
	tmp[4] = lineA.s456789ab;
	tmp[5] = lineA.s56789abc;
	tmp[6] = lineA.s6789abcd;	
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[0][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight += weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut += tmpPixel * weight;
	}
	
	tmp[0] = lineB.s01234567;
	tmp[1] = lineB.s12345678;
	tmp[2] = lineB.s23456789;
	tmp[3] = lineB.s3456789a;
	tmp[4] = lineB.s456789ab;
	tmp[5] = lineB.s56789abc;
	tmp[6] = lineB.s6789abcd;
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[1][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight += weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut += tmpPixel * weight;
	}
	
	tmp[0] = lineC.s01234567;
	tmp[1] = lineC.s12345678;
	tmp[2] = lineC.s23456789;
	tmp[3] = lineC.s3456789a;
	tmp[4] = lineC.s456789ab;
	tmp[5] = lineC.s56789abc;
	tmp[6] = lineC.s6789abcd;
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[2][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight += weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut += tmpPixel * weight;
	}
	
	tmp[0] = lineD.s01234567;
	tmp[1] = lineD.s12345678;
	tmp[2] = lineD.s23456789;
	tmp[3] = lineD.s3456789a;
	tmp[4] = lineD.s456789ab;
	tmp[5] = lineD.s56789abc;
	tmp[6] = lineD.s6789abcd;
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[3][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight += weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut += tmpPixel * weight;
	}
	
	lineA = vload16(0, input_image + (y + 1)  * WIDTH + x * 8 - 3);
    lineB = vload16(0, input_image + (y + 2)  * WIDTH + x * 8 - 3);
    lineC = vload16(0, input_image + (y + 3)  * WIDTH + x * 8 - 3);
	tmp[0] = lineA.s01234567;
	tmp[1] = lineA.s12345678;
	tmp[2] = lineA.s23456789;
	tmp[3] = lineA.s3456789a;
	tmp[4] = lineA.s456789ab;
	tmp[5] = lineA.s56789abc;
	tmp[6] = lineA.s6789abcd;
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[4][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight += weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut += tmpPixel * weight;
	}
	
	tmp[0] = lineB.s01234567;
	tmp[1] = lineB.s12345678;
	tmp[2] = lineB.s23456789;
	tmp[3] = lineB.s3456789a;
	tmp[4] = lineB.s456789ab;
	tmp[5] = lineB.s56789abc;
	tmp[6] = lineB.s6789abcd;
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[5][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight += weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut += tmpPixel * weight;
	}
	
	tmp[0] = lineC.s01234567;
	tmp[1] = lineC.s12345678;
	tmp[2] = lineC.s23456789;
	tmp[3] = lineC.s3456789a;
	tmp[4] = lineC.s456789ab;
	tmp[5] = lineC.s56789abc;
	tmp[6] = lineC.s6789abcd;
	for(int i = 0; i < 7; i++){
		rangeWeight0 = abs_diff(tmp[i], lineP);
		rangeWeight1 = convert_float8(rangeWeight0);
		rangeWeight2 = rangeWeight1 * rangeWeight1 / (SPCW * 2);
		spaceWeight = (float8)med1[6][i];
		weight = exp(-(rangeWeight2 + spaceWeight));
		sumWeight = sumWeight + weight;
		tmpPixel = convert_float8(tmp[i]);
		tmpOut = tmpOut + tmpPixel * weight;
	}
	tmpOut = tmpOut / sumWeight;
    out = convert_uchar8(tmpOut);
    vstore8(out, x, output_image + y * WIDTH);
}
__kernel void yuyv2rgba( __global const uchar * src1, __global const uchar * src2, __global uchar * dst)
{ 
	int i = get_global_id(0);
	int j = get_global_id(1);
	int y_idx = mad24(j, WIDTH, i);
	int uv_idx = mad24(j, WIDTH * 2, i*2);
	int dst_idx = mad24(j, WIDTH * 4, i*4);
	unsigned int temp;
	float4 outColor; 				
    float4 out = (float4)( src1[y_idx], src2[uv_idx], src2[uv_idx + 1], 255.0);
	
	temp = (out.x -16)*1.164 + 1.596 * (out.z - 128); 
	outColor.z = (temp < 0 ? 0 : temp);
	outColor.z = (temp > 255 ? 255 : temp);
	
	temp = (out.x - 16)*1.164  - 0.392 * (out.y - 128) - 0.813 * (out.z - 128);
	outColor.y = (temp < 0 ? 0 : temp);
	outColor.y = (temp > 255 ? 255 : temp);
	
	temp = (out.x - 16)*1.164  + 2.017 * (out.y - 128);
	outColor.x = (temp < 0 ? 0 : temp);
	outColor.x = (temp > 255 ? 255 : temp);
	
	outColor.w = 255.0;
	
    uchar4 outColor1= convert_uchar4(outColor);
    vstore4(outColor1, 0, &dst[dst_idx]);

}
