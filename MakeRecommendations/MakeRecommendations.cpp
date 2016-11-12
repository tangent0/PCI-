
/**
http://www.wuzesheng.com/?p=1277
（1）推荐系统通常可以分为两类：一类是基于人的推荐系统，它利用人与人之间的相似度来进行推荐；一类是基于物品的推荐，它利用物品之间的相似度来进行推荐。
   通俗地讲，基于人的推荐，是通过分析人与人喜欢的物品，计算出人与人之间的相似度，然后做推荐；而基于物品的推荐，是通过分析某人喜欢的物品与其它物品的相似度，
   然后来为其做推荐。
（2）推荐系统中比较关键的算法就是相似度的计算，有人与人之间的相似度计算，也有物品与物品之间的相似度计算。相似度计算函数要满足如下特点：
    拥有同样的函数签名，以一个浮点数做为返回值，其数值越大代表相似度越大。下面介绍几个算法：
    a. 欧几里德距离（Euclidean Distance），我们知道两个人的喜好越相似，他们的欧几里德距离值越小，
	   所以需要将欧几里德距离转化一下，这里介绍一个简单的转化：1/(1 + dist)， 这样就能保证越相似取值越大，而且取值范围在（0, 1]
    b. 皮求逊相关系数（Pearson Correlation Coefficient）, 取值范围[-1, 1], 越相似值越大，满足条件
    c. Tanimoto系统 （Tanimoto Coefficient），最值范围[0, 1], 越相似值越大，满足条件
（3）在推荐系统里，需要注意：
    a. 没有对某物品进行评价的人不能对该物品的推荐打分产生影响
    b. 不能因为某人的偏执喜好（打很高或者很低的分）对推荐打分产生明显的影响
  为了避免上面的问题，通常采用加权平均的方法来计算某物品的推荐打分（详见第三部分算法实现）

  本系统采用的数据是来自http://www.grouplens.org/node/73的数据，本次实验采用的是如下图所示的第三份数据，总共有6040个用户和3952部电影，以及1000209条相关评价。


  我们用三种不同的打分函数，为第1000个用户，推荐20部电影，来对比一下推荐的结果：
  a. Euclidean结果：
  b. Pearson结果：
  c. Tanimoto结果：

  从上面的结果中，可以得出如下结果：
  a. 采用Euclidean打分推荐和采用Pearson打分推荐的结果中，有16个是相同的
  b. 采用Pearson打分推荐和采用Tanimoto打分推荐的结果中，有15个是相同的
  c. 采用Tanimoto打分推荐和采用Euclidean打分推荐的结果中，有16个是相同的
  从上面的数据可以看出，虽说采用不同的打分函数进行推荐的结果存在一定的差异，但是整体上是一致的，不同的结果的相互覆盖率都超过了75%, 这说明我们的打分函数都还是比较有效的。

*/


//E:\Projects\PCI\src
#define arg_exist -1

//#include "..\src\Score.h"
//#include "..\src\RecommendData.h"

#include "Score.h"
#include "RecommendData.h"

#include <map>
#include <fstream>
#include <iostream>

#include<time.h>

using namespace std;

const char * g_pMovieData = "./million-ml-data/movies.dat";
const char * g_pUserData = "./million-ml-data/users.dat";
const char * g_pRatingData = "./million-ml-data/ratings.dat";

map<int, Movie> g_mapMovieInfo;
map<int, User> g_mapUserInfo;
map<int, map<int, Rating> > g_mapRatingInfo;
double ** g_szRatings = NULL;

int main(int argc, char ** argv)
{
	//
	printf("main begins\n");
	

#if(arg_exist==1) 
	if (argc != 3)
	{
		cerr << "Usage: " << argv[0] << " personIndex recNum\n";
		return 1;
	}
#endif	
	ifstream oFileIn;
	string strLine;

	//counting time
	clock_t start_time;
	clock_t end_time;
	clock_t cost_time;

	///load movie info
	start_time = clock();
	cout << "load movie info begins" << endl;
	oFileIn.open(g_pMovieData, ios::in);
	Movie oMovie;
	while (getline(oFileIn, strLine))
	{
		Parser::ParseMovie(strLine, oMovie);
		g_mapMovieInfo[oMovie.GetID()] = oMovie;
		
		//cout << strLine << endl;
	}
	oFileIn.clear();
	oFileIn.close();
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "load movie info finished. Costing time: "<< cost_time<<"ms \n"<<endl;


	///load user info
	start_time = clock();
	cout << "load user info begins" << endl;
	oFileIn.open(g_pUserData, ios::in);
	User oUser;
	while (getline(oFileIn, strLine))
	{
		Parser::ParseUser(strLine, oUser);
		g_mapUserInfo[oUser.GetID()] = oUser;

		//cout << strLine << endl;
	}
	oFileIn.clear();
	oFileIn.close();
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "load user info finished. Costing time: " << cost_time << "ms \n" << endl;
	
	/**/
	///load rating
	start_time = clock();
	cout << "load rating info begins" << endl;
	oFileIn.open(g_pRatingData, ios::in);
	Rating oRating;
	while (getline(oFileIn, strLine))
	{
		Parser::ParseRating(strLine, oRating);
		g_mapRatingInfo[oRating.GetUserID()][oRating.GetMovieID()] = oRating;

		//cout << strLine << endl;
	}
	oFileIn.close();
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "load rating info finished. Costing time: " << cost_time << "ms \n" << endl;

	size_t unMaxUserNum = g_mapUserInfo.rbegin()->first;
	size_t unMaxMovieNum = g_mapMovieInfo.rbegin()->first;

	//size_t unMaxUserNum = 7000;
	//size_t unMaxMovieNum = 4000;

	///allocate memory
	start_time = clock();
	cout << "allocate memory begins" << endl;
	g_szRatings = new double*[unMaxUserNum];	
	for (size_t unIdx = 0; unIdx < unMaxUserNum; ++ unIdx)
	{
		g_szRatings[unIdx] = new double[unMaxMovieNum];
		::memset(g_szRatings[unIdx], 0, sizeof(double)*unMaxMovieNum);
	}
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "allocate memory finished. Costing time: " << cost_time << "ms \n" << endl;
	
	///get critics
	start_time = clock();
	cout << "allocate memory begins" << endl;
	map<int, User>::iterator oItUser = g_mapUserInfo.begin();
	while (oItUser != g_mapUserInfo.end())
	{
		map<int, Rating>::iterator oItRating = g_mapRatingInfo[oItUser->first].begin();
		while (oItRating != g_mapRatingInfo[oItUser->first].end())
		{
			g_szRatings[oItUser->first - 1][oItRating->first - 1] = oItRating->second.GetRate();
			++ oItRating;
		}
		++ oItUser;
	}
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "get critics finished. Costing time: " << cost_time << "ms \n" << endl;

//when arg_exits==-1, default value as for nPersonIndex=999, nRecNum=20 in project main argument settings
#if(arg_exist==1)
	int nPersonIndex = ::atoi(argv[1]) - 1;
	int nRecNum = ::atoi(argv[2]);
#elif(arg_exist==0)
	int nPersonIndex;
	int nRecNum;
	cout << "input nPersonIndex, nRecNum: ";
	cin >> nPersonIndex >> nRecNum;
	nPersonIndex = nPersonIndex - 1;
#elif(arg_exist==-1)
	int nPersonIndex = 1000 - 1;
	int nRecNum = 20;
#endif

	int * pRecItems = new int[nRecNum];
	double * pRecScores = new double[nRecNum];

	///Euclidean
	start_time = clock();
	GetRecommendation(const_cast<const double **>(g_szRatings), unMaxUserNum, 
		unMaxMovieNum, nPersonIndex, GetEuclideanScore, nRecNum, pRecItems, pRecScores);
	cout << "---------------------Euclidean Recommendation: \n";
	for (int nIdx = 0; nIdx < nRecNum; ++ nIdx)
	{
		cout << nIdx << "\t" << pRecScores[nIdx] << "\t" 
			<< g_mapMovieInfo[pRecItems[nIdx]].GetTitle() << endl;
	}
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "count finished. Costing time : " << cost_time << "ms" << endl;

	///Pearson
	start_time = clock();
	GetRecommendation(const_cast<const double **>(g_szRatings), unMaxUserNum, 
		unMaxMovieNum, nPersonIndex, GetPearsonScore, nRecNum, pRecItems, pRecScores);
	cout << "\n---------------------Pearson Recommendation: \n";
	for (int nIdx = 0; nIdx < nRecNum; ++ nIdx)
	{
		cout << nIdx << "\t" << pRecScores[nIdx] << "\t" 
			<< g_mapMovieInfo[pRecItems[nIdx]].GetTitle() << endl;
	}
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "count finished. Costing time : " << cost_time << "ms" << endl;

	///cosine
	start_time = clock();
	GetRecommendation(const_cast<const double **>(g_szRatings), unMaxUserNum, 
		unMaxMovieNum, nPersonIndex, GetCosineScore, nRecNum, pRecItems, pRecScores);
	cout << "\n---------------------Cosine Recommendation: \n";
	for (int nIdx = 0; nIdx < nRecNum; ++ nIdx)
	{
		cout << nIdx << "\t" << pRecScores[nIdx] << "\t" 
			<< g_mapMovieInfo[pRecItems[nIdx]].GetTitle() << endl;
	}
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "count finished. Costing time : " << cost_time << "ms" << endl;

	/**/
	///Tanimoto
	start_time = clock();
	GetRecommendation(const_cast<const double **>(g_szRatings), unMaxUserNum,
		unMaxMovieNum, nPersonIndex, GetTanimotoScore, nRecNum, pRecItems, pRecScores);
	cout << "\n---------------------Tanimoto Recommendation: \n";
	for (int nIdx = 0; nIdx < nRecNum; ++nIdx)
	{
		cout << nIdx << "\t" << pRecScores[nIdx] << "\t"
			<< g_mapMovieInfo[pRecItems[nIdx]].GetTitle() << endl;
	}
	end_time = clock();
	cost_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "count finished. Costing time : " << cost_time << "ms" << endl;

	///deallocate memory
	for (size_t unIdx = 0; unIdx < unMaxUserNum; ++ unIdx)
	{
		delete [] g_szRatings[unIdx];
	}
	delete [] g_szRatings;

	printf("main end.\n");
	system("pause");
}