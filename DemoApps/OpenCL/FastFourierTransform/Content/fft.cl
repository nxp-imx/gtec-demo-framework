#define M_PI 3.14159265358979f
#define MUL_RE(a,b) (a.even*b.even - a.odd*b.odd)
#define MUL_IM(a,b) (a.even*b.odd + a.odd*b.even)

typedef float2 real2_t;
typedef float real_t;

// complex multiply
real2_t mul(
    real2_t a,
    real2_t b
    )
{
	return (real2_t)(a.x*b.x-a.y*b.y,a.x*b.y+a.y*b.x); // no mad
}

// twiddle_P_Q(A) returns A * EXP(-P*PI*i/Q)
real2_t
twiddle_1_2(
    real2_t a
    )
{
	// A * (-i)
	return (real2_t)(a.y,-a.x);
}

// Return A * exp(K*ALPHA*i)
real2_t
twiddle(
    real2_t a,
    int k,
    real_t alpha
    )
{
	real_t cs,sn;
	//sn = sincos((real_t)k*alpha,&cs);
    cs = native_cos((real_t) k * alpha);
    sn = native_sin((real_t) k * alpha);

	return mul(a,(real2_t)(cs,sn));
}

// Return A * exp(KALPHA*i)
real2_t
twiddle_kalpha(
    real2_t a,
    real_t kalpha
    )
{
	real_t cs,sn;
    //sn = sincos((real_t) alpha, &cs);
    cs = native_cos((real_t) kalpha);
    sn = native_sin((real_t) kalpha);
	return mul(a,(real2_t)(cs,sn));
}

// In-place DFT-2, output is (a,b). Arguments must be variables.
#define DFT2(a,b) { real2_t tmp = a - b; a += b; b = tmp; }

// Compute T x DFT-2.
// T is the number of threads.
// N = 2*T is the size of input vectors.
// X[N], Y[N]
// P is the length of input sub-sequences: 1,2,4,...,T.
// Each DFT-2 has input (X[I],X[I+T]), I=0..T-1,
// and output Y[J],Y|J+P], J = I with one 0 bit inserted at postion P. */
__kernel void
fft_radix2(
    __global const real2_t * x,
    __global real2_t * y,
    int p,
    int pminus1,
    real_t minusPIoverp
    )
{
	int t = get_global_size(0); // thread count
	int i = get_global_id(0);   // thread index
	int k = i&pminus1;          // index in input sequence, in 0..P-1
	int j = ((i-k)<<1) + k;     // output index
	real_t alpha = minusPIoverp*(real_t)k;// -M_PI*(real_t)k/(real_t)p;

	// Read and twiddle input
	x += i;
	real2_t u0 = x[0];
//	real2_t u1 = twiddle(x[t],1,alpha);
	real_t cs,sn;
	//sn = sincos(alpha,&cs);
    cs = native_cos(alpha);
    sn = native_sin(alpha);
	real2_t u1 = mul(x[t],(real2_t)(cs,sn));

	// In-place DFT-2
	DFT2(u0,u1);

	// Write output
	y += j;
	y[0] = u0;
	y[p] = u1;
}

// In-place DFT-4, output is (a,c,b,d). Arguments must be variables.
#define DFT4(a,b,c,d) { DFT2(a,c); DFT2(b,d); d=twiddle_1_2(d); DFT2(a,b); DFT2(c,d); }

// Compute T x DFT-4.
// T is the number of threads.
// N = 4*T is the size of input vectors.
// X[N], Y[N]
// P is the length of input sub-sequences: 1,4,16,...,T.
// Each DFT-4 has input (X[I],X[I+T],X[I+2*T],X[I+3*T]), I=0..T-1,
// and output (Y[J],Y|J+P],Y[J+2*P],Y[J+3*P], J = I with two 0 bits inserted at postion P.
__kernel void
fft_radix4(
    __global const float2 * x,
    __global float2 * y,
    int p,
    int pminus1,
    int twop,
    int threep,
    real_t minusPIover2p,
    real_t minusPIover2p_2x,
    real_t minusPIover2p_3x
    )
{
	int t = get_global_size(0); // thread count
	int i = get_global_id(0); // thread index
	int k = i&pminus1;//(p-1); // index in input sequence, in 0..P-1
	int j = ((i-k)<<2) + k; // output index
	real_t alpha = minusPIover2p*(real_t)k;//-M_PI*(real_t)k/(real_t)(2*p);
	real_t alpha2x = minusPIover2p_2x*(real_t)k;
	real_t alpha3x = minusPIover2p_3x*(real_t)k;

	// Read and twiddle input
	x += i;
	real2_t u0 = x[0];
	real2_t u1 = twiddle_kalpha(x[t],alpha);//twiddle(x[t],1,alpha);
	real2_t u2 = twiddle_kalpha(x[2*t],alpha2x);//twiddle(x[2*t],2,alpha);
	real2_t u3 = twiddle_kalpha(x[3*t],alpha3x);//twiddle(x[3*t],3,alpha);

	// In-place DFT-4
	DFT4(u0,u1,u2,u3);

	// Shuffle and write output
	y        += j;
	y[0]      = u0;
	y[p]      = u2;
	y[twop]   = u1;
	y[threep] = u3;
}

