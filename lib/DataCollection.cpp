/**
 * @file DataCollection.cpp
 * @brief 
 * @author liqun (liqun@nudt.edu.cn)
 * @version 1.0
 * @date 2020-09-10
 * 
 * @copyright Copyright(C) 2020 liqun
 * 
 */
#include "Error.h"
#include "DataCollection.h"

using namespace std;
using namespace rubber_duck;

void DataCollection::printHeading() {
    string t(120,'-');
    cout << t.c_str() << endl;
    cout << setiosflags(ios::left) 
        << setw(20) << "TITLE" //"性能指标" 
        << setw(10) << "(RE)SET" //"统计开始时间" 
        << setw(10) << "OBS."    //"观察数量"
        << setw(16) << "MEAN"   //"均值"
        << setw(16) << "STDEV"  //"方差"
        << setw(16) << "MINIMUM"//"最小值"
        << setw(16) << "MAXIMUM"//"最大值"
        << setw(16) << "CONFIDENCE%95"  //"置信区间（%95）"    
        << resetiosflags(ios::left) << endl;
    cout << t.c_str() << endl;
}

void DataCollection::printEnding() {
    string t(120,'-');
    cout << t.c_str() << endl;
}

void DataCollection::report() {
    cout << setiosflags(ios::left) 
        << setw(20) << title 
        << setw(10) << resetAt 
        << setw(10) << obs
        << setw(16) << mean()
        << setw(16) << stdDev()
        << setw(16) << min()
        << setw(16) << max()
        << setw(16) << confidence()    
        << resetiosflags(ios::left) << endl;
}    

double DataCollection::confidence(double level) {
    int df = obs - 1;
    double t = tValue((1 - level) / 2, df);
    if (t == std::numeric_limits<double>::max())
        return t;
    return (t * stdDev()) / sqrt(df);
}

double DataCollection::tValue(double p, int df) {
    p = p >= 0.5 ? 1 - p : p;
    if (p <= 0 || df == 0)
        return std::numeric_limits<double>::max();
    if (p == 0.5)
        return 0;
    double t;
    if (df == 1)
        t = 1 / tan((p + p) * 1.57079633);
    else if (df == 2)
        t = sqrt(1 / ((p + p) * (1 - p)) - 2);
    else {
        double a = sqrt(log(1 / (p * p)));
        double aa = a * a;
        a -= (2.515517 + (0.802853 * a) + (0.010328 * aa)) / 
                (1.0 + (1.432788 * a) + (0.189269 * aa) + (0.001308 * aa * a));
        t = df - 0.666666667 + 1.0 / (10.0 * df);
        t = sqrt (df * (exp (a * a * (df - 0.833333333) / (t * t)) - 1.0));
    }
    return abs(t);
} 

void Tally::reset(double time) {
    obs = 0;
    sum = sumsq = m_min = m_max = 0;
    resetAt = time;
}

void Tally::update(double v,double time) { 
    obs++;
    sum += v;
    sumsq += v * v;
    if (obs == 1)
        m_min = m_max = v;
    else if (v < m_min)
        m_min = v;
    else if (v > m_max)
        m_max = v;
}
    
double Tally::variance() {
    return abs((sumsq - sum * sum / obs) / obs);
}

double Tally::mean() {
    if (obs == 0)
        return 0;
    return sum / obs;
}

void Accumulate::reset(double time) {
    obs = 0;
    sum = sumsq = m_min = m_max = 0;
    lastTime = resetAt = time;
}

void Accumulate::update(double v,double time) {
    obs++;
    double t = time;
    double span = t - lastTime;
    sum += lastV * span;
    sumsq += v * v * span;
    lastTime = t;
    lastV = v;
    if (obs == 1)
        m_min = m_max = v;
    else if (v < m_min)
        m_min = v;
    else if (v > m_max)
        m_max = v;
}
    
double Accumulate::mean() {
    double span = lastTime - resetAt;
    if (span == 0)
        return 0;
    return sum / span;
}
    
double Accumulate::variance() {
    double span = lastTime - resetAt;
    if (span == 0)
        return 0;
    return abs(sumsq / span - pow(mean(), 2));
}

Histogram::Histogram(const char * title, double lower, double upper, int nCells):Tally(title){
    if (lower >= upper)
        err(HISTUPPER);
    if (nCells < 1)
        err(HISTCELLS);
    width = (upper - lower) / nCells;
    limit = nCells + 1;
    this->lower = lower;
    this->upper = upper;
    this->nCells = nCells;
    table = new int[nCells + 2];
}
    
Histogram::~Histogram(){
    delete table;
}

void Histogram::reset(double time) {
    Tally::reset(time);
    for (int cell = 0; cell < limit; cell++) 
        table[cell] = 0;
}
    
void Histogram::update(double v,double time) {
    Tally::update(v,time);
    v -= lower;
    int cell;
    if (v < 0)
        cell = 0;
    else {
        cell = (int) round(v / width) + 1;
        if (cell > limit)
            cell = limit;
    }
    table[cell]++;
}
    
void Histogram::report() {
    Tally::report();
    printEnding();
    cout << setiosflags(ios::right) 
        << setw(10) << "CELLs"      //"区间" 
        << setw(10) << "LOWER"      //"区间下限" 
        << setw(10) << "COUNT"      //"区间数量"
        << setw(10) << "FREQUENCY"  //"频率"
        << setw(10) << "CUM %"      //"累积频率"   
        << resetiosflags(ios::right) << endl << endl;
    double max = table[0];
    for (int cell = 1; cell <= limit; cell++)
        if (table[cell] > max)
            max = table[cell];
    double scale = 40 / max;
    double sum = 0;
    int occ = 0;

    for (int cell = 0; cell <= limit; cell++) {
        cout << setiosflags(ios::right) << setw(10) << cell;
        if (cell == 0) 
            cout  << setw(10) << "-infinity";
        else
            cout  << setw(10) << lower + (cell - 1) * width;
        int next = table[cell];
        cout  << setw(10) << next;
        double freq = (double) next / obs;
        cout  << setw(10) << setprecision(2) << freq;
        sum += freq * 100;
        cout  << setw(10) << fixed << setprecision(2) << sum << setw(3) << "|";        
        if (next > 0) {
            string t((int) round(scale * next),'*');
            cout << t.c_str() << endl;
        }
        cout << endl;
        occ += next;
        if (occ == obs && cell + 3 > limit) {
            break;
        }
    }
}