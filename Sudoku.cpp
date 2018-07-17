#include "stdafx.h"
#include <iostream>
#include <fstream>
#define N 9
#define NREL 20
#define INPUTFILENAME "inputf.dat"
#define OUTPUTFILENAME "outputf.dat"

class Sudoku{
private:
	int nRest;
	int Grid[N][N];
	int nCandidate[N][N];
	bool Candidate[N][N][N + 1];

	void RelGrid(int ylist[], int xlist[], int y, int x){
		int it = 0;
		bool key[N * N] = { false };
		key[y * N + x] = true;
		for (int i = 0; i < N; i++)
			if (!key[i * N + x]){
				key[i * N + x] = true;
				ylist[it] = i;
				xlist[it++] = x;
			}
		for (int j = 0; j < N; j++)
			if (!key[y * N + j]){
				key[y * N + j] = true;
				ylist[it] = y;
				xlist[it++] = j;
			}
		int block_y0 = y - (y % 3);
		int block_x0 = x - (x % 3);
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (!key[(block_y0 + i) * N + block_x0 + j]){
					key[(block_y0 + i) * N + block_x0 + j] = true;
					ylist[it] = block_y0 + i;
					xlist[it++] = block_x0 + j;
				}
	}

	void BelGrid(int ylist[], int xlist[], int y, int x, char option){
		int it = 0;
		int block_y0, block_x0;
		switch (option)
		{
		case 'R':
			for (int j = 0; j < N; j++){
				ylist[it] = y;
				xlist[it++] = j;
			}
			break;
		case 'C':
			for (int i = 0; i < N; i++){
				ylist[it] = i;
				xlist[it++] = x;
			}
			break;
		case 'B':
			block_y0 = y - (y % 3);
			block_x0 = x - (x % 3);
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++){
					ylist[it] = block_y0 + i;
					xlist[it++] = block_x0 + j;
				}
			break;
		default:
			break;
		}
	}

	void ModCand(int y, int x){
		int ylist[NREL], xlist[NREL];
		RelGrid(ylist, xlist, y, x);
		for (int it = 0; it < NREL; it++)
			if (Candidate[ylist[it]][xlist[it]][Grid[y][x]]){
				Candidate[ylist[it]][xlist[it]][Grid[y][x]] = false;
				nCandidate[ylist[it]][xlist[it]]--;
			}
		return;
	}

	bool Exclude(char option, bool *flag){
		int ylist[N], xlist[N];
		for (int i = 0; i < N; i++){
			switch (option)
			{
			case 'R':
				BelGrid(ylist, xlist, i, 0, 'R');
				break;
			case 'C':
				BelGrid(ylist, xlist, 0, i, 'C');
				break;
			case 'B':
				BelGrid(ylist, xlist, (i / 3) * 3, (i % 3) * 3, 'B');
				break;
			default:
				return false;
				break;
			}
			int numCand[N + 1][2] = { 0 };
			for (int it = 0; it < N; it++)
				for (int k = 1; k < N + 1; k++)
					if (Candidate[ylist[it]][xlist[it]][k]){
						numCand[k][0]++;
						numCand[k][1] = it;
					}
			for (int k = 1; k < N + 1; k++)
				if (numCand[k][0] <= 0){
					return false;
				}
				else if (numCand[k][0] == 1){
					if (!Grid[ylist[numCand[k][1]]][xlist[numCand[k][1]]]){
						Grid[ylist[numCand[k][1]]][xlist[numCand[k][1]]] = k;
						nRest--;
						for (int kk = 1; kk < N + 1; kk++)
							Candidate[ylist[numCand[k][1]]][xlist[numCand[k][1]]][kk] = false;
						Candidate[ylist[numCand[k][1]]][xlist[numCand[k][1]]][k] = true;
						nCandidate[ylist[numCand[k][1]]][xlist[numCand[k][1]]] = 1;
						ModCand(ylist[numCand[k][1]], xlist[numCand[k][1]]);
						*flag = true;
					}
				}
				else if (option != 'B')
					if (numCand[k][0] <= 3){
						bool inOneBlk = true;
						for (int it = 0; it < N; it++)
							if (Candidate[ylist[it]][xlist[it]][k]){
								if (ylist[it] / 3 != ylist[numCand[k][1]] / 3)
									inOneBlk = false;
								if (xlist[it] / 3 != xlist[numCand[k][1]] / 3)
									inOneBlk = false;
							}
						if (inOneBlk){
							int yylist[N], xxlist[N];
							BelGrid(yylist, xxlist, ylist[numCand[k][1]], xlist[numCand[k][1]], 'B');
							for (int it = 0; it < N; it++)
								if (((option == 'R') && (yylist[it] != i)) || ((option == 'C') && (xxlist[it] != i)))
									if (Candidate[yylist[it]][xxlist[it]][k]){
										Candidate[yylist[it]][xxlist[it]][k] = false;
										nCandidate[yylist[it]][xxlist[it]]--;
										*flag = true;
									}
						}
					}
		}
		return true;
	}

	bool Check(){
		for (int y = 0; y < N; y++)
			for (int x = 0; x < N; x++)
				if (Grid[y][x]){
					bool Conflict = false;
					int ylist[NREL], xlist[NREL];
					RelGrid(ylist, xlist, y, x);
					for (int it = 0; it < NREL; it++)
						if (Grid[ylist[it]][xlist[it]] == Grid[y][x])
							Conflict = true;
					if (Conflict)
						return false;
				}
		return true;
	}

	bool Refresh(int y, int x){
		int ylist[NREL], xlist[NREL];
		RelGrid(ylist, xlist, y, x);
		for (int it = 0; it < NREL; it++)
			if (Grid[ylist[it]][xlist[it]])
				Candidate[y][x][Grid[ylist[it]][xlist[it]]] = false;
		for (int k = 1; k < N + 1; k++)
			if (!Candidate[y][x][k])
				nCandidate[y][x]--;
		if (!nCandidate[y][x])
			return false;
		else
			return true;
	}

	bool Initialize(){
		if (!Check())
			return false;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				if (Grid[i][j]){
					nCandidate[i][j] = 1;
					for (int k = 1; k < N + 1; k++)
						Candidate[i][j][k] = false;
					Candidate[i][j][Grid[i][j]] = true;
				}
				else{
					bool flag = Refresh(i, j);
					if (!flag)
						return false;
				}
				return true;
	}

	bool FillGrid(int *yBest, int *xBest, int *nBest){
		bool flag;
		do{
			flag = false;
			*nBest = 0;
			if (!(Exclude('R', &flag) && Exclude('C', &flag) && Exclude('B', &flag))) return false;
			for (int y = 0; y < N; y++)
				for (int x = 0; x < N; x++)
					if (nCandidate[y][x] <= 0){
						return false;
					}
					else if (nCandidate[y][x] == 1){
						if (!Grid[y][x]){
							for (int k = 1; k < N + 1; k++)
								if (Candidate[y][x][k]){
									Grid[y][x] = k;
									nRest--;
									break;
								}
							ModCand(y, x);
							flag = true;
						}
					}
					else{
						if ((!(*nBest)) || (nCandidate[y][x] < (*nBest))){
							*nBest = nCandidate[y][x];
							*yBest = y;
							*xBest = x;
						}
					}
						
		} while (flag);
		return true;
	}

public:
	int Answer[N][N];

	Sudoku(){
		nRest = N * N;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++){
				Grid[i][j] = 0;
				nCandidate[i][j] = N;
				for (int k = 0; k < N + 1; k++)
					Candidate[i][j][k] = true;
			}
	}

	~Sudoku(){
		//Nothing
	}

	bool Input(){
		using namespace std;
		ifstream fin(INPUTFILENAME);
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++){
				fin >> Grid[i][j];
				if (Grid[i][j])
					nRest--;
			}
		fin.close();
		return Initialize();
	}

	bool Dfs(int depth){
		int newNRest;
		int newNCandidate[N][N];
		bool newCandidate[N][N][N + 1];
		int yBest, xBest, nBest;
		if (!FillGrid(&yBest, &xBest, &nBest))
			return false;
		if (nRest == 0){
			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
					Answer[i][j] = Grid[i][j];
			return true;
		}
		for (int k = 1; k < N + 1; k++)
			if (Candidate[yBest][xBest][k]){
				newNRest = nRest;
				for (int i = 0; i < N; i++)
					for (int j = 0; j < N; j++){
						newNCandidate[i][j] = nCandidate[i][j];
						for (int kk = 0; kk < N + 1; kk++)
							newCandidate[i][j][kk] = Candidate[i][j][kk];
					}
				nRest--;
				Grid[yBest][xBest] = k;
				ModCand(yBest, xBest);
				if (Dfs(depth + 1))
					return true;
				nRest++;
				Grid[yBest][xBest] = 0;
				for (int i = 0; i < N; i++)
					for (int j = 0; j < N; j++){
						nCandidate[i][j] = newNCandidate[i][j];
						for (int kk = 0; kk < N + 1; kk++)
							Candidate[i][j][kk] = newCandidate[i][j][kk];
					}
			}
		return false;
	}
};

void Output(int Answer[N][N], bool flag){
	using namespace std;
	if (!flag){
		cout << "Invalid Input!" << endl;
		return;
	}
	else{
		ofstream fout(OUTPUTFILENAME);
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++)
				fout << Answer[i][j] << " ";
			fout << endl;
		}
		fout.close();
		cout << "Done!" << endl;
	}
	return;
}

int main()
{
	Sudoku puzzle;
	if (!puzzle.Input()){
		Output(nullptr, false);
		return -1;
	}
	if (!puzzle.Dfs(0)){
		Output(nullptr, false);
		return -1;
	}
	Output(puzzle.Answer, true);
    return 0;
}