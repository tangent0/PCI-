
/**
http://www.wuzesheng.com/?p=1277
��1���Ƽ�ϵͳͨ�����Է�Ϊ���ࣺһ���ǻ����˵��Ƽ�ϵͳ��������������֮������ƶ��������Ƽ���һ���ǻ�����Ʒ���Ƽ�����������Ʒ֮������ƶ��������Ƽ���
   ͨ�׵ؽ��������˵��Ƽ�����ͨ������������ϲ������Ʒ�������������֮������ƶȣ�Ȼ�����Ƽ�����������Ʒ���Ƽ�����ͨ������ĳ��ϲ������Ʒ��������Ʒ�����ƶȣ�
   Ȼ����Ϊ�����Ƽ���
��2���Ƽ�ϵͳ�бȽϹؼ����㷨�������ƶȵļ��㣬��������֮������ƶȼ��㣬Ҳ����Ʒ����Ʒ֮������ƶȼ��㡣���ƶȼ��㺯��Ҫ���������ص㣺
    ӵ��ͬ���ĺ���ǩ������һ����������Ϊ����ֵ������ֵԽ��������ƶ�Խ��������ܼ����㷨��
    a. ŷ����¾��루Euclidean Distance��������֪�������˵�ϲ��Խ���ƣ����ǵ�ŷ����¾���ֵԽС��
	   ������Ҫ��ŷ����¾���ת��һ�£��������һ���򵥵�ת����1/(1 + dist)�� �������ܱ�֤Խ����ȡֵԽ�󣬶���ȡֵ��Χ�ڣ�0, 1]
    b. Ƥ��ѷ���ϵ����Pearson Correlation Coefficient��, ȡֵ��Χ[-1, 1], Խ����ֵԽ����������
    c. Tanimotoϵͳ ��Tanimoto Coefficient������ֵ��Χ[0, 1], Խ����ֵԽ����������
��3�����Ƽ�ϵͳ���Ҫע�⣺
    a. û�ж�ĳ��Ʒ�������۵��˲��ܶԸ���Ʒ���Ƽ���ֲ���Ӱ��
    b. ������Ϊĳ�˵�ƫִϲ�ã���ܸ߻��ߺܵ͵ķ֣����Ƽ���ֲ������Ե�Ӱ��
  Ϊ�˱�����������⣬ͨ�����ü�Ȩƽ���ķ���������ĳ��Ʒ���Ƽ���֣�������������㷨ʵ�֣�

  ��ϵͳ���õ�����������http://www.grouplens.org/node/73�����ݣ�����ʵ����õ�������ͼ��ʾ�ĵ��������ݣ��ܹ���6040���û���3952����Ӱ���Լ�1000209��������ۡ�


  ���������ֲ�ͬ�Ĵ�ֺ�����Ϊ��1000���û����Ƽ�20����Ӱ�����Ա�һ���Ƽ��Ľ����
  a. Euclidean�����
  b. Pearson�����
  c. Tanimoto�����

  ������Ľ���У����Եó����½����
  a. ����Euclidean����Ƽ��Ͳ���Pearson����Ƽ��Ľ���У���16������ͬ��
  b. ����Pearson����Ƽ��Ͳ���Tanimoto����Ƽ��Ľ���У���15������ͬ��
  c. ����Tanimoto����Ƽ��Ͳ���Euclidean����Ƽ��Ľ���У���16������ͬ��
  ����������ݿ��Կ�������˵���ò�ͬ�Ĵ�ֺ��������Ƽ��Ľ������һ���Ĳ��죬������������һ�µģ���ͬ�Ľ�����໥�����ʶ�������75%, ��˵�����ǵĴ�ֺ��������ǱȽ���Ч�ġ�

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