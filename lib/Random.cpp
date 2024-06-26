/**
 * @file Random.cpp
 * @brief 随机变量生成对象类Random实现
 * @author liqun (liqun@nudt.edu.cn)
 * @version 2.0
 * @date 2020-10-23
 * 
 * @copyright Copyright(C) 2020 liqun
 */
#include "iostream"
#include <cfloat>
#include <math.h>
#include <assert.h>
#include "Random.h"
#include "Error.h"

#ifndef M_E
#define M_E     2.7182818284590452353602874713527
#endif

using namespace std;
using namespace rubber_duck;

Random::Random(unsigned long int seed){
	e = new std::mt19937_64(seed);
}

Random::~Random(){
	delete e;
}

double Random::nextDouble(){
	return u(*e);
}

int Random::nextInteger(int lower, int upper){
  return (int)(nextDouble() * (upper - lower ) + lower);
}

double Random::nextDouble(double lower, double upper ){
	if (upper < lower) err (UNFRNG);
	return (nextDouble() * ( upper - lower ) + lower );
}

double Random::nextExponential(double p){
	return (-p * log(nextDouble()));
}

double Random::nextUnitNormalBM (){
  double U = nextDouble();
  double V = nextDouble();
  return sqrt(-2.0 * log(U)) * cos(M_PI * 2 * V);
}

double Random::nextNormalBM (double  mean, double  stdDev ){
	double U = nextDouble();
  double V = nextDouble();
  return mean + stdDev * sqrt(-2.0 * log(U)) * cos(M_PI * 2 * V);
}

double Random::nextTruncNormal(double  mean, double  stdDev ){
  double res;
  do {
    res = nextNormal(mean, stdDev);
  } while (res < 0);
  return res;
}

double Random::nextNormal (double  mean, double  stdDev){
	double  u, v1, v2, s, x1;

	if (mean < 0.0)   err (NEGNMLMN);
	if (stdDev < 0.0) err (NEGNMLSD);

	do{
		u  = nextDouble();
		v1 = 2.0 * u  - 1.0;
		u  = nextDouble();
		v2 = 2.0 * u - 1.0;
		s = v1 * v1 + v2 * v2;
	} while (s >= 1.0 || s == 0.0);

	x1 = v1 * sqrt((-2.0 * log(s)) / s);
	return (mean + x1 * stdDev);
}

double Random::nextChiSquare (unsigned int n ){
	return this->nextGamma ( 0.5 * n, 2);
}

double Random::nextStudentT (unsigned int n ){
	return ( this->nextNormal ( 0.0 , 1.0 ) / sqrt( this->nextChiSquare (n) / n) );
}

double Random::nextLogNormal (double  mean, double  stdDev ){
	if (mean < 0.0)   err (NEGLNMLMN);
	if (stdDev < 0.0) err (NEGLNMLSD);

	return exp( this->nextNormal(mean, stdDev));
}

double Random::nextErlang (unsigned int k, double m){
  if (k < 7) {
		/******************************************/
		/* use k series exponential distributions */
		/******************************************/
		double U = 1.0;
		for (unsigned int i = 0; i < k; i++)
			U *= nextDouble();
		return -(m / (double)k) * log(U);
	}
	else {
    /*************/
		/* use gamma */
		/*************/
		return this->nextGamma ((double)k, m);
  }
}

/*
 * internal, for alpha>1.
 *
 * From: "A Simple Method for Generating Gamma Variables", George Marsaglia and
 * Wai Wan Tsang, ACM Transactions on Mathematical Software, Vol. 26, No. 3,
 * September 2000. Available online.
 */
double Random::gamma_Marsaglia2000(double a)
{
  assert(a > 1);
  double d, c, x, v, u;
  d = a - 1.0 / 3.0;
  c = 1.0 / sqrt(9.0 * d);
  for (;;){
  do{
    x = nextUnitNormalBM();
    v = 1.0 + c * x;
  } while (v <= 0);
  v = v * v * v;
  u = nextDouble();
  if (u < 1.0 - 0.0331 * (x * x) * (x * x))
    return d * v;
  if (log(u) < 0.5 * x * x + d * (1.0 - v + log(v)))
    return d * v;
  }
}

/*
 * internal, for alpha<1.
 *
 * We can derive the alpha<1 case from the alpha>1 case. See "Note" at the
 * end of Section 6 of the Marsaglia2000 paper (see above).
 */
double Random::gamma_MarsagliaTransf(double alpha){
  assert(alpha < 1);
  return gamma_Marsaglia2000(1 + alpha) * pow(nextDouble(), 1 / alpha);  // faster
}


double Random::nextGamma (double  alpha, double  beta ){
  if (alpha <= 0 || beta <= 0)
    err (NEGALPHA);

  if (fabs(alpha - 1.0) <= DBL_EPSILON) {
    return nextExponential(beta);
  }
  else if (alpha < 1.0) {
    // return theta * gamma_AhrensDieter74(rng, alpha); // implementation is bogus, see above
    return beta * gamma_MarsagliaTransf(alpha);
  }
  else {  // if (alpha > 1.0)
    // return theta * gamma_ChengFeast79(rng, alpha);
    return beta * gamma_Marsaglia2000(alpha);  // faster
  }
}

double Random::nextBeta (double  alpha, double  beta ){
	double z;

	if (beta == 0.0) {
		err (EQUZERO);
	}

	z = this->nextGamma(alpha, 1.0);

	return ( z / (z + this->nextGamma( beta, 1.0)) );
}

double Random::nextF (unsigned int n1, unsigned int n2 ){
  return ( (n2 * this->nextChiSquare (n1)) / ( n1 * this->nextChiSquare (n2)));
}

/**
 * @brief Returns a random integer from the Poisson distribution with parameter lambda,
 * that is, the number of arrivals over unit time where the time between
 * successive arrivals follow exponential distribution with parameter lambda.
 *
 * Lambda is also the mean (and variance) of the distribution.
 *
 * Generation method depends on value of lambda:
 *
 *   - 0<lambda<=30: count number of events
 *   - lambda>30: Acceptance-Rejection due to Atkinson (see Banks, page 166)
 *
 * @param lambda  > 0
 */

double Random::nextPoisson (double  lambda ){
  int X;
  if (lambda > 30.0){
    double a = M_PI * sqrt(lambda / 3.0);
    double b = a / lambda;
    double c = 0.767 - 3.36 / lambda;
    double d = log(c) - log(b) - lambda;
    double U, V, Y;

    do{
      do{
        U = nextDouble();
        Y = (a - log((1.0 - U) / U)) / b;
      } while (Y <= -0.5);

      X = (int)floor(Y + 0.5);
      V = nextDouble();
    } while (a - b * Y + log(V / 1.0 + pow(exp(a - b * Y), 2.0)) > d + X * log(lambda) - log(double(X)));
    //应该是X的阶乘，需要验证
  }
  else{
    double a = exp(-lambda);
    double p = 1.0;
    X = -1;

    while (p > a){
      p *= nextDouble();
      X++;
    }
  }
  return X;
}

double Random::nextGeometric(double p){
	if ((p <= 0.0) || (p >= 1.0))
		err(PROBVAL);
  double a = 1.0 / (log(1.0 - p));
  return (int)floor(a * log(nextDouble()));
}

double Random::nextWeibull (double  alpha, double  beta ){
	return beta * pow(-log(nextDouble()), 1.0 / alpha);
}

double Random::nextBinomial(double  p, unsigned int n ){
  int X = 0;
  // sum up n bernoulli trials
  for (unsigned int i = 0; i < n; i++) {
    double U = nextDouble();
    if (p > U)
      X++;
  }
  return X;
}

double Random::nextNegBinomial (double  p, unsigned int  n){
	if ((p <= 0.0) || (p >= 1.0))
		err(PROBVAL);

  int X = 0;
  for (unsigned int i = 0; i < n; i++) {
    X += (int)nextGeometric(p);
  }
  return X;
}

double Random::nextTriang (double a, double b, double c){
	if (b < a || c < b || a == c) err (RNGTRGMD);
  double U, beta, T;

  U = nextDouble();
  beta = (b - a) / (c - a);

  if (U < beta)
    T = sqrt(beta * U);
  else
    T = 1.0 - sqrt((1.0 - beta) * (1.0 - U));

  return a + (c - a) * T;
}

bool Random::probability (double  p ){
  if ((p < 0.0) || (p > 1.0)) err (PROBVAL);
	return (p >= nextDouble());
}

//浮点数相等的比较
#define fequal(a,b) (fabs((a)-(b))<0.000001)

CDFTable * rubber_duck::makeCDFTable (int number, double  *x_axis, double  *y_axis){
  CDFTable *ts;
  int i;
  double x, y;

  y = y_axis[0];
  /*
  ** Take the left boundary of first histogram column.
  */
  x = x_axis[0];

  for (i = 1;i < number;i++){
    if (y > y_axis[i]) err (TABMONO );
      y = y_axis[i];

    if (x > x_axis[i]) err (TABINC );
      x = x_axis[i];
  }

  if (y_axis[number - 2] != 1.0) err (TABYRNG );

  ts = new CDFTable;
  ts->size = number - 1;
  ts->x_axis = x_axis;
  ts->y_axis = y_axis;
  return ts;
} 

CDFDiscreteTable * rubber_duck::makeDiscreteCDFTable (int number, int valuelist[], double problist[]){
   CDFDiscreteTable * ts;

	for (int i = 1;i < number;i ++){
		problist[i] = problist[i - 1] + problist[i];
	}

	if(!fequal(problist[number - 1],1.0)){
		err(CDFERROR);
	}

  ts = new CDFDiscreteTable;
  ts->size = number;
  ts->x_axis = valuelist;
  ts->y_axis = problist;
  return ts;
} 

double Random::nextContinuous(CDFTable * pTable,double p){
  double  grad, x1, x2, y1,y2;
  int i;

  if (!pTable)
    err(NULLTAB);
  if(p < 0){
    p = nextDouble();
  }

	for (int j = 0;j < pTable->size;j ++){
		if (p <= pTable->y_axis[j]){
			i = j;
      break;
		}
	}
  /*
  ** Get first point
  */
  x1 = pTable->x_axis[i];
  if (i == 0)
    y1 = 0;
  else
    y1 = pTable->y_axis[i - 1];
  y2 = pTable->y_axis[i];
  /*
  ** Get second point (y2 = ts->y_axis[i])
  */
  x2 = pTable->x_axis[i + 1];

  /*
  ** Use linear interpolation
  */
  grad = (x2 - x1)/(y2 - y1);

  return (x1 + (p - y1) * grad);
}

int Random::nextDiscrete(CDFDiscreteTable * pTable,double p){
  if (!pTable)
    err(NULLTAB);

  double u = p;
  if(u < 0){
	  u = nextDouble();
  }

	for (int i = 0;i < pTable->size;i ++){
		if (u <= pTable->y_axis[i]){
			return pTable->x_axis[i];
		}
	}

	assert(0);
	return pTable->x_axis[pTable->size - 1];
}

/*
void main()
{
	int i;
	Generator *generator = new Generator(1,0);
	cout<<generator->Exponential()<<endl;
	cout<<generator->Uniform01();
	cin>>i;
}
*/
