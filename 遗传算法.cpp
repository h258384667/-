#include <iostream>
#include <bitset>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
using namespace std;

#define SIZE 20//染色体长度，根据实际情况改写

typedef bitset<SIZE> Chromosome;
typedef vector<Chromosome> Population;
typedef vector<Chromosome> MatingPool;


void cross(MatingPool& pool, double crossRate)
{
	default_random_engine e(time(0));
	uniform_real_distribution<double> rand_r(0, 1);
	uniform_int_distribution<int> rand_p(1, SIZE - 1);
	for (int i = 0; i < pool.size(); i += 2)
	{
		if (rand_r(e) <= crossRate)
		{
			int pos = rand_p(e);
			Chromosome temp = pool[i] >> pos << pos | pool[i + 1] << (SIZE - pos) >> (SIZE - pos);
			pool[i + 1] = pool[i + 1] >> pos << pos | pool[i] << (SIZE - pos) >> (SIZE - pos);
			pool[i] = temp;
		}
	}
}


void mutate(Population& population, double mutationRate)
{
	default_random_engine e(time(0));
	uniform_real_distribution<double> rand_r(0, 1);
	uniform_int_distribution<int> rand_p(1, SIZE - 1);
	for (int i = 0; i < population.size(); i++)
	{
		if (rand_r(e) <= mutationRate)
		{
			population[i].flip(rand_p(e));
		}
	}
}

double parse(Chromosome chromo)
{
	double value = chromo.to_ullong();
	value /= (1 << SIZE) - 1;
	value *= 3.141592654;
	return value;
}

//价值函数，根据实际情况改写
double value_of(Chromosome chromo)
{
	double value = cos(parse(chromo));
	return 1 / value / value;
}


void Scale(vector<double>& values)
{
	vector<double> temp;
	for (int i = 0; i < values.size(); i++)
	{
		int rank = values.size();
		for (int j = 0; j < values.size(); j++)
		{
			if (i == j) continue;
			if (values[i] < values[j]) rank--;
		}
		temp.push_back(rank);
	}
	values.swap(temp);
}


void Init(Population& population, int popSize)
{
	popSize = popSize >> 1 << 1;
	default_random_engine e(time(0));
	uniform_int_distribution<int> b(0, 1);
	for (int i = 0; i < popSize; i++)
	{
		Chromosome chromo;
		for (int i = 0; i < SIZE; i++)
		{
			chromo.set(i, b(e));
		}
		population.push_back(chromo);
	}
}


void selection(Population population, vector<double> values, MatingPool& pool)
{
	Scale(values);
	int size = population.size();
	for (int i = 1; i < size; i++) values[i] += values[i - 1];
	default_random_engine e(time(0));
	uniform_real_distribution<double> rand(0, values[size - 1]);
	for (int i = 0; i < size; i++)
	{
		double pos = rand(e);
		int index = 0;
		while (values[index] < pos) index++;
		pool.push_back(population[index]);
	}
}


void assessment(Population population, vector<double>& values, int& best, int& worst)
{
	values.clear();
	double temp = value_of(population[0]); values.push_back(temp);
	int max(temp), min(temp), size(population.size());
	best = worst = 0;
	for (int i = 1; i < size; i++)
	{
		double value = value_of(population[i]); values.push_back(value);
		if (max < value)
		{
			max = value;
			best = i;
		}
		if (min > value)
		{
			min = value;
			worst = i;
		}
	}
}


bool eponch(Population& population, vector<double>& values, int& index_last_elite, int& continue_count,double crossRate, double mutationRate, int limit)
{
	//提取上一代的精英
	double lastBestValue = values[index_last_elite];
	Chromosome last_elite = population[index_last_elite];
	//交叉重组变异
	MatingPool pool; selection(population, values, pool);
	cross(pool, crossRate);
	population.swap(pool);
	mutate(population, mutationRate);
	//评估当前种群
	int best, worst; assessment(population, values, best, worst);
	//精英保留
	double curBestValue = values[best];
	if (curBestValue < lastBestValue)
	{
		curBestValue = lastBestValue;
		best = worst;
		population[best] = last_elite;
		values[best] = lastBestValue;
	}
	index_last_elite = best;
	//判断终止条件
	continue_count = curBestValue == lastBestValue ? continue_count + 1 : 0;
	return continue_count < limit;
}


void showPop(Population population)
{
	for (int i = 0; i < population.size(); i++)
		cout << population[i] << " "<<value_of(population[i]) << endl;
	cout << endl;
}


void run(int popSize, double crossRate, double mutationRate,int limit)
{
	Population population; Init(population, popSize); 
	//showPop(population);
	vector<double> values;
	int best, worst;
	assessment(population, values, best, worst);
	int current_count = 0;
	while (eponch(population, values, best, current_count, crossRate, mutationRate, limit));
	//showPop(population);
	cout << "best:" << population[best] << " value:" << value_of(population[best]) << " parse:" << parse(population[best]) << endl;
	double x = parse(population[best]);
	cout << "x=" << x << " cos(x)=" << cos(x) << endl;
}


int main()
{
	while (1)
	{
		run(50, 0.8, 0.001, 200);
		system("pause");
	}
	return 0;
}