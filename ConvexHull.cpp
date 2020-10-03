#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <chrono>
#include <set>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Point
{
	int x;
	int y;
	Point* cw_next;		// the next clockwise point 
	Point* ccw_next;	// the next counter-clockwise point 
	Point(int _x, int _y) : x(_x), y(_y), cw_next(NULL), ccw_next(NULL)
	{}
};

// To find orientation of ordered triplet (p, q, r)
// return value:
// 0 --> p, q and r are colinear
// 1 --> Clockwise
//-1 --> Counterclockwise
int orientation(Point* p, Point* q, Point* r)
{
	int val = (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
	if (val == 0)
		return 0;
	return (val > 0) ? 1 : -1;
}

// Jarvis March Algorithm 
double elapsedTime = 0.0;
vector<Point*> JavisMarchAlgor(const vector<Point*> points)
{
	auto start = chrono::steady_clock::now();

	int size = (int)points.size();
	// There must be at least 3 points
	if (size < 3)
		return points;
	
	// Initial with -1
	vector<int> nextIndex(size, -1);
	// Find the leftmost point
	int l = 0;
	for (int i = 1; i < size; i++)
		if (points[i]->x < points[l]->x)
			l = i;

	// Start from left most point, keep moving counter clockwise, until reach the start point again
	int p = l, q;
	do
	{
		// Search for a point 'q' such that orientation(p, i, q) is counterclockwise for all points 'i'
		q = (p + 1) % size;
		for (int i = 0; i < size; i++)
			if (orientation(points[p], points[i], points[q]) < 0)
				q = i;

		nextIndex[p] = q; // Add q to result as a next point of p
		p = q; // Set p as q for next iteration
	} while (p != l);

	vector<Point*> result;
	for (auto tmpInd : nextIndex)
	{
		if (tmpInd >= 0)
			result.push_back(points[tmpInd]);
	}

	auto end = chrono::steady_clock::now();
	elapsedTime = chrono::duration<double>(end - start).count();
	return result;
}

// Quick Hull Algorithm 
// Compare two points, determine whether point a < b
bool ComparePoint(Point* a, Point* b) 
{
	if (a->x == b->x)
		return a->y < b->y;
	return a->x < b->x;
}

// Find the upper or lower hull
vector<Point*> QuickHalfHull(vector<Point*> originSet, Point* p1, Point* p2, bool findUpper)
{
	vector<Point*> upperSet;
	Point* maxPoint = NULL;
	int maxDistance = -1;
	for (auto iter = originSet.begin(); iter != originSet.end(); iter++)
	{
		int distance = p1->x * p2->y + ((*iter)->x) * p1->y + p2->x * ((*iter)->y) - ((*iter)->x) * p2->y - p2->x * p1->y - p1->x * ((*iter)->y);
		if ((findUpper && distance > 0) || (!findUpper && distance < 0))
		{
			upperSet.push_back(*iter);
			distance = std::abs(distance);
			if (distance > maxDistance) {
				maxPoint = (*iter);
				maxDistance = distance;
			}
		}
	}

	if (maxDistance > 0) 
	{
		vector<Point*> SubHull1 = QuickHalfHull(upperSet, p1, maxPoint, findUpper);
		vector<Point*> SubHull2 = QuickHalfHull(upperSet, maxPoint, p2, findUpper);
		SubHull1.insert(SubHull1.begin(), SubHull2.begin(), SubHull2.end());
		SubHull1.push_back(maxPoint);
		return SubHull1;
	}
	return vector<Point*>();	// empty
}

vector<Point*> QuickHullAlgor(vector<Point*> originSet) 
{
	auto start = chrono::steady_clock::now();

	// Sort all points
	sort(originSet.begin(), originSet.end(), ComparePoint);

	// Find upper and lower hulls
	vector<Point*> SubHull1 = QuickHalfHull(originSet, originSet.front(), originSet.back(), true);
	vector<Point*> SubHull2 = QuickHalfHull(originSet, originSet.front(), originSet.back(), false);

	// combine upper and lower hulls
	SubHull1.insert(SubHull1.begin(), SubHull2.begin(), SubHull2.end());
	SubHull1.push_back(originSet.front());
	SubHull1.push_back(originSet.back());

	auto end = chrono::steady_clock::now();
	elapsedTime = chrono::duration<double>(end - start).count();

	return SubHull1;
}

// Merge Hull Algorithm 
// build clock order and counter clock order of the points, when it can not be devided any more 
vector<Point*> InitSubHull(vector<Point*>& points)
{
	// it is very like Jarvis March scan 
	int size = int(points.size());
	map<int, int> nextIndex;
	// Find the leftmost point
	int l = 0;
	for (int i = 1; i < size; i++)
		if (points[i]->x < points[l]->x)
			l = i;

	int p = l, q;
	do
	{
		// Search for a point 'q' such that orientation(p, i, q) is counterclockwise for all points 'i'
		q = (p + 1) % size;
		for (int i = 0; i < size; i++)
			if (orientation(points[p], points[i], points[q]) < 0)
				q = i;
		nextIndex[p] = q; // Add q to result as a next point of p
		p = q; // Set p as q for next iteration
	} while (p != l);

	vector<Point*> result;
	p = l;
	do 
	{
		Point* tmpPoint = points[p];
		Point* nextPoint = points[nextIndex[p]];
		tmpPoint->ccw_next = nextPoint;
		nextPoint->cw_next = tmpPoint;
		result.push_back(tmpPoint);
		p = nextIndex[p];
		if (p == l)
			break;
	} while (1);
	return result;
}

vector<Point*> Merge(vector<Point*> chull1, vector<Point*> chull2)
{
	Point *p = NULL, *q = NULL;
	int xRight = -10000, xLeft = 10000;
	for (auto tmpPoint : chull1)
	{
		if (tmpPoint->x > xRight)
		{
			p = tmpPoint;
			xRight = tmpPoint->x;
		}
	}
	for (auto tmpPoint : chull2)
	{
		if (tmpPoint->x < xLeft)
		{
			q = tmpPoint;
			xLeft = tmpPoint->x;
		}
	}

	Point* cp_p = p;
	Point* cp_q = q;
	Point* prev_p = NULL, *prev_q = NULL;
	while (1)
	{
		prev_p = p;
		prev_q = q;
		if (q->cw_next)
		{
			while (orientation(q, p, q->cw_next) > 0)
				q = q->cw_next;
		}
		if (p->ccw_next)
		{
			while (orientation(p, q, p->ccw_next) < 0)
				p = p->ccw_next;
		}
		if (p == prev_p && q == prev_q)
			break;
	}

	prev_p = NULL;
	prev_q = NULL;

	while (1)
	{
		prev_p = cp_p;
		prev_q = cp_q;
		if (cp_q->ccw_next)
		{
			while (orientation(cp_q, cp_p, cp_q->ccw_next) < 0)
				cp_q = cp_q->ccw_next;
		}
		if (cp_p->cw_next)
		{
			while (orientation(cp_p, cp_q, cp_p->cw_next) > 0)
				cp_p = cp_p->cw_next;
		}
		if (cp_p == prev_p && cp_q == prev_q)
			break;
	}

	// Remove all other points 
	p->cw_next = q;
	q->ccw_next = p;
	cp_p->ccw_next = cp_q;
	cp_q->cw_next = cp_p;

	vector<Point*> result;
	Point* start = p;
	while (1)
	{
		result.push_back(p);
		p = p->ccw_next;
		if (p == start)
			break;
	}
	return result;
}

bool CheckValidSubHull(const vector<Point*>& points)
{
	if (points.size() < 3)
		return false;
	int x = points[0]->x;
	for (const auto p : points)
	{
		if (p->x != x)
			return true;
	}
	return false;
}

vector<Point*> MergeHull(vector<Point*> originSet)
{
	int size = (int)originSet.size();
	if (size < 6)	// can not split anymore 
		return InitSubHull(originSet);

	vector<Point*> leftPart, rightPart;
	int halfSize = size / 2;
	int halfX;
	for (int i = 0; i < size; ++i)
	{
		if (i == halfSize - 1)
			halfX = originSet[i]->x;
		if (i < halfSize || originSet[i]->x == halfX)
			leftPart.push_back(originSet[i]);
		else
			rightPart.push_back(originSet[i]);		
	}
	bool leftValid = CheckValidSubHull(leftPart);
	bool rightValid = CheckValidSubHull(rightPart);
	if (!leftValid || !rightValid)
		return InitSubHull(originSet);

	vector<Point*> leftResult = MergeHull(leftPart);
	vector<Point*> rightResult = MergeHull(rightPart);
	return Merge(leftResult, rightResult);
}

vector<Point*> MergeHullAlgor(vector<Point*> origSet)
{
	auto start = chrono::steady_clock::now();

	// Sort all points
	sort(origSet.begin(), origSet.end(), ComparePoint);
	vector<Point*> result = MergeHull(origSet);

	auto end = chrono::steady_clock::now();
	elapsedTime = chrono::duration<double>(end - start).count();

	return result;
}

set<pair<int, int> > allPoints;
bool ReadFile(string fileName, int* number)
{
	*number = 0;
	ifstream ifs(fileName);
	if (!ifs)
	{
		cerr << "Can not open file: " << fileName << "!" << endl;
		return false;
	}
	int x, y;
	while (ifs >> x >> y)
	{
		allPoints.insert(pair<int, int>(x, y));
		(*number)++;
	}
	return true;
}

void GenData(int size)
{
	// x random range [0, 1000); y random range [0, 1000)
	if (size > 100000)
	{
		cerr << "Too many points!" << endl;
		exit(0);
	}
	srand((unsigned int)time(NULL));
	while ((int)allPoints.size() < size)
	{
		int x = rand() % 1000;
		int y = rand() % 1000;
		allPoints.insert(pair<int, int>(x, y));
	}
}

void DebugPrint(vector<Point*> points)
{
	sort(points.begin(), points.end(), ComparePoint);
	for (auto p : points)
		cout << "[" << p->x << "," << p->y << "] ";
	cout << endl;
}

int main(int argc, char* argv[])
{
	int number;
	while (1)
	{
		string option, fileName;
		cout << "Please select input file[1] or random generation[2]: ";
		cin >> option;
		cin.ignore(1024, '\n');
		bool fail = true;

		if (option == "1")
		{
			cout << "Please input file name: ";
			cin >> fileName;
			cin.ignore(1024, '\n');
			fail = !ReadFile(fileName, &number);
		}
		else if (option == "2")
		{ 
			cout << "Please input points number: ";
			cin >> number;
			cin.ignore(1024, '\n');
			if (number <= 2)
				cout << "Invalid points number!" << endl;
			else
			{
				GenData(number);
				fail = false;
			}
		}
		if (!fail)
			break;
	}

	vector<Point*> origPoints;
	for (auto it = allPoints.begin(), ie = allPoints.end(); it != ie; ++it)
		origPoints.push_back(new Point(it->first, it->second));

	char strFileName[100];
	sprintf(strFileName, "set%d.out", number);
	ofstream ofs(strFileName);
	double t1, t2, t3;
	vector<Point*> result1 = JavisMarchAlgor(origPoints);
	t1 = elapsedTime;
	// cout << "Jarvis March result:";
	// DebugPrint(result1);
	vector<Point*> result2 = QuickHullAlgor(origPoints);
	t2 = elapsedTime;
	// cout << "QuickHull result:";
	// DebugPrint(result2);
	vector<Point*> result3 = MergeHullAlgor(origPoints);
	t3 = elapsedTime;
	// cout << "MergeHull result:";
	// DebugPrint(result3);

	ofs << "Jarvis March: " << t1 << " sec" << endl
		<< "QuickHull: " << t2 << " sec" << endl
		<< "MergeHull: " << t3 << " sec" << endl;

	for (auto p : result2)
		ofs << p->x << " " << p->y << endl;

	return 0;
}