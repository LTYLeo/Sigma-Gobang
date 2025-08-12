#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include<windows.h>
#include<limits>
#include<map> // ������в�洢

using namespace std;

const int N = 15; // ���̴�С
const char ChessBoardFlag = ' ';
const char flag1 = 'O'; // ���
const char flag2 = 'X'; // AI

// λ�ü�ֵ��
const int positionValue[N + 1][N + 1] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
    {0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5, 6, 6, 6, 6, 5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0},
    {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// ��в���Ͷ���
enum ThreatType {
    NONE = 0,
    SINGLE = 1,     // ����
    OPEN_TWO = 2,   // ���
    HALF_OPEN_TWO = 3, // �߶�
    OPEN_THREE = 4, // ����
    HALF_OPEN_THREE = 5, // ����
    OPEN_FOUR = 6,  // ����
    HALF_OPEN_FOUR = 7, // ����
    FIVE = 8        // ����
};

typedef struct Coordinate {
    int x;
    int y;
} ChessCoordi;

class FiveChess {
public:
    FiveChess() {
        InitChessBoard();
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        lastComputerMove = {0, 0};
        gameOver = false;
    }

    void Play() {
        ChessCoordi Pos1;
        ChessCoordi Pos2;
        while (1) {
            gameOver = false;
            int mode = ChoseMode();
            size_t count = 1;

            while (!gameOver) {
                if (mode == 1) { // ���VS����
                    PalyerGo(Pos1, 1, flag1);
                    if (count >= 9 && GetWiner(Pos1, 1, flag1)) {
                        gameOver = true;
                    }
                    if (!gameOver) {
                        ComputerGo(Pos2, flag2);
                        lastComputerMove = Pos2;
                        if (count >= 9 && GetWiner(Pos2, 0, flag2)) {
                            gameOver = true;
                        }
                        count++;
                    }
                } else if (mode == 2) { // ���VS���
                    PalyerGo(Pos1, 1, flag1);
                    if (count >= 9 && GetWiner(Pos1, 1, flag1)) {
                        gameOver = true;
                    }
                    if (!gameOver) {
                        PalyerGo(Pos2, 2, flag2);
                        if (count >= 9 && GetWiner(Pos2, 2, flag2)) {
                            gameOver = true;
                        }
                    }
                }
                count++;
            }

            cout << "����һ�� y or no" << endl;
            char chose = 'y';
            cin >> chose;
            if (chose == 'n')
                break;
            else {
                InitChessBoard();
                lastComputerMove = {0, 0};
            }
        }
    }

    void PrintChessBoard() {
        system("cls");
        for (size_t i = 0; i < N + 1; ++i) {
            for (size_t j = 0; j < N + 1; ++j) {
                if (i == 0) {
                    if (j != 0)
                        printf("%d   ", j);
                    else if (j == 0)
                        printf("  ");
                } else if (j == 0) {
                    if (i != 0)
                        printf("%2d", i);
                } else {
                    if (i == lastComputerMove.x && j == lastComputerMove.y) {
                        SetConsoleTextAttribute(hConsole, 14); // ��ɫ
                        cout << " " << ChessBoard[i][j] << " ";
                        SetConsoleTextAttribute(hConsole, 7); // Ĭ��
                        cout << "|";
                    } else {
                        cout << " " << ChessBoard[i][j] << " |";
                    }
                }
            }
            cout << endl;
            cout << "  ";
            for (size_t i = 1; i < N + 1; ++i) {
                cout << "---+";
            }
            cout << endl;
        }
    }

    void InitChessBoard() {
        for (size_t i = 0; i < N + 1; ++i) {
            for (size_t j = 0; j < N + 1; ++j) {
                ChessBoard[i][j] = ChessBoardFlag;
            }
        }
        step_count = 0;
        gameOver = false;
    }

protected:
    int ChoseMode() {
        system("cls");
        InitChessBoard();
        cout << "��ѡ�� 1.���VS���� 2.���VS��� 3.�˳�" << endl;
        int chose = 0;
        cin >> chose;
        while (1) {
            if (chose == 1)
                return chose;
            else if (chose == 2)
                return chose;
            else if (chose == 3)
                exit(1);
            else
                cout << "�Բ��� �����������󡣡�" << endl;
        }
    }

    void PalyerGo(ChessCoordi& Pos, int player, char flag) {
        PrintChessBoard();
        int x = 0;
        int y = 0;
        while (1) {
            cout << "�����" << player << "��һ����" << endl;
            cin >> Pos.x >> Pos.y;
            if (JudgePos(Pos))
                break;
            else
                cout << "���������� ! ����������" << endl;
        }
        ChessBoard[Pos.x][Pos.y] = flag;
        step_count++;
    }

    // �߼�AI���Ӻ��������ಽԤ�У�
    void ComputerGo(ChessCoordi& Pos, char flag) {
        // ��һ����������λ��
        if (step_count == 0) {
            Pos.x = N / 2 + 1;
            Pos.y = N / 2 + 1;
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }

        char opponent = (flag == flag1) ? flag2 : flag1;
        
        // 1. ����Ƿ���������ʤ�Ļ���
        if (FindImmediateWin(Pos, flag)) {
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }
        
        // 2. ����Ƿ���Ҫ�������أ�����л��ġ����Ļ�˫��������в��
        if (FindCriticalDefensiveMove(Pos, opponent)) {
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }
        
        // 3. �ಽԤ�У��������п���λ��
        int bestScore = INT_MIN;
        ChessCoordi bestMove;
        vector<ChessCoordi> candidateMoves;
        
        // ��ȡ���к�ѡλ��
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (ChessBoard[i][j] == ChessBoardFlag) {
                    // ֻ������������Χ������
                    if (HasNeighbor(i, j, 2)) {
                        candidateMoves.push_back({i, j});
                    }
                }
            }
        }
        
        // ���û�к�ѡλ�ã����ѡ��
        if (candidateMoves.empty()) {
            while (1) {
                Pos.x = rand() % N + 1;
                Pos.y = rand() % N + 1;
                if (ChessBoard[Pos.x][Pos.y] == ChessBoardFlag)
                    break;
            }
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }
        
        // ����ÿ����ѡλ��
        for (const auto& move : candidateMoves) {
            int score = EvaluatePosition(move.x, move.y, flag, opponent);
            
            // �ಽԤ�У����Ƕ��ֵ�Ӧ��
            ChessBoard[move.x][move.y] = flag;
            int opponentScore = EvaluateBestResponse(opponent, flag);
            ChessBoard[move.x][move.y] = ChessBoardFlag;
            
            // ���շ��� = �ҷ��÷� - ���ֵ÷� * ����ϵ��
            int finalScore = score - opponentScore * 0.7;
            
            if (finalScore > bestScore) {
                bestScore = finalScore;
                bestMove = move;
            }
        }
        
        Pos = bestMove;
        ChessBoard[Pos.x][Pos.y] = flag;
        step_count++;
    }

    // ����Ƿ���������ʤ�Ļ���
    bool FindImmediateWin(ChessCoordi& move, char flag) {
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (ChessBoard[i][j] == ChessBoardFlag) {
                    ChessBoard[i][j] = flag;
                    if (CheckWin(i, j, flag)) {
                        move = {i, j};
                        return true;
                    }
                    ChessBoard[i][j] = ChessBoardFlag;
                }
            }
        }
        return false;
    }

    // ���ؼ�����λ�ã����ġ����ġ�˫�����ȣ�
    bool FindCriticalDefensiveMove(ChessCoordi& move, char opponent) {
        // �洢������вλ��
        map<pair<int, int>, int> threatMap;
        
        // ɨ����������Ѱ����в
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (ChessBoard[i][j] == ChessBoardFlag) {
                    int threatLevel = EvaluateThreatLevel(i, j, opponent);
                    threatMap[{i, j}] = threatLevel;
                }
            }
        }
        
        // Ѱ����߼������в
        int maxThreat = 0;
        ChessCoordi bestDefense;
        for (const auto& threat : threatMap) {
            if (threat.second > maxThreat) {
                maxThreat = threat.second;
                bestDefense = {threat.first.first, threat.first.second};
            }
        }
        
        // ����и���в�����ġ����ġ�˫�������������
        if (maxThreat >= 10000) {
            move = bestDefense;
            return true;
        }
        
        return false;
    }

    // ������в����
    int EvaluateThreatLevel(int x, int y, char opponent) {
        ChessBoard[x][y] = opponent;
        int threatLevel = 0;
        
        // ���ֱ�ӻ�ʤ��в
        if (CheckWin(x, y, opponent)) {
            threatLevel = 20000; // ��߼�����в
            ChessBoard[x][y] = ChessBoardFlag;
            return threatLevel;
        }
        
        // ��������в
        if (CheckOpenFour(x, y, opponent)) {
            threatLevel = 15000;
        }
        // ��������в
        else if (CheckHalfOpenFour(x, y, opponent)) {
            threatLevel = 10000;
        }
        // ���˫������в
        else if (CheckDoubleOpenThree(x, y, opponent)) {
            threatLevel = 8000;
        }
        // ��������в
        else if (CheckOpenThree(x, y, opponent)) {
            threatLevel = 5000;
        }
        
        ChessBoard[x][y] = ChessBoardFlag;
        return threatLevel;
    }

    // ����λ�ü�ֵ�����ǹ�����
    int EvaluatePosition(int x, int y, char self, char opponent) {
        int score = positionValue[x][y]; // λ�û�����ֵ
        
        // ��ʱ�������ӽ�������
        ChessBoard[x][y] = self;
        score += EvaluateLine(x, y, self, opponent);
        
        // ����������в
        ChessBoard[x][y] = opponent;
        score += EvaluateLine(x, y, opponent, self) * 1.2; // ����Ȩ�ظ���
        
        ChessBoard[x][y] = ChessBoardFlag;
        
        return score;
    }
    
    // �����С��С��Խ��ߵļ�ֵ
    int EvaluateLine(int x, int y, char self, char opponent) {
        int score = 0;
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}}; // �ĸ�����
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            // �������
            int value = EvaluatePattern(x, y, dx, dy, self, opponent);
            score += value;
        }
        
        return score;
    }
    
    // �����ض������ϵ�����
    int EvaluatePattern(int x, int y, int dx, int dy, char self, char opponent) {
        int score = 0;
        int selfCount = 1;  // ��ǰλ���Ѿ�������self
        int openEnds = 0;
        bool leftOpen = true, rightOpen = true;
        
        // ������
        for (int i = 1; i <= 4; i++) {
            int nx = x - i * dx;
            int ny = y - i * dy;
            
            if (nx < 1 || nx > N || ny < 1 || ny > N) {
                leftOpen = false;
                break;
            }
            
            if (ChessBoard[nx][ny] == self) {
                selfCount++;
            } else if (ChessBoard[nx][ny] == opponent) {
                leftOpen = false;
                break;
            } else {
                break; // ��λ
            }
        }
        
        // ���Ҽ��
        for (int i = 1; i <= 4; i++) {
            int nx = x + i * dx;
            int ny = y + i * dy;
            
            if (nx < 1 || nx > N || ny < 1 || ny > N) {
                rightOpen = false;
                break;
            }
            
            if (ChessBoard[nx][ny] == self) {
                selfCount++;
            } else if (ChessBoard[nx][ny] == opponent) {
                rightOpen = false;
                break;
            } else {
                break; // ��λ
            }
        }
        
        // ���㿪�Ŷ˿�
        if (leftOpen) openEnds++;
        if (rightOpen) openEnds++;
        
        // �����������Ϳ��Ŷ˿�����
        switch (selfCount) {
            case 5: // ����
                score += 100000;
                break;
            case 4:
                if (openEnds == 2) score += 10000; // ����
                else if (openEnds == 1) score += 1000; // ����
                break;
            case 3:
                if (openEnds == 2) score += 1000; // ����
                else if (openEnds == 1) score += 200; // ����
                break;
            case 2:
                if (openEnds == 2) score += 200; // ���
                else if (openEnds == 1) score += 50; // �߶�
                break;
            case 1:
                if (openEnds == 2) score += 10; // ����
                break;
        }
        
        return score;
    }

    // ����Ƿ��ʤ
    bool CheckWin(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1; // ��ǰλ��
            
            // ������
            for (int i = 1; i <= 4; i++) {
                int nx = x + i * dx;
                int ny = y + i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N || ChessBoard[nx][ny] != flag) {
                    break;
                }
                count++;
            }
            
            // ������
            for (int i = 1; i <= 4; i++) {
                int nx = x - i * dx;
                int ny = y - i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N || ChessBoard[nx][ny] != flag) {
                    break;
                }
                count++;
            }
            
            if (count >= 5) {
                return true;
            }
        }
        
        return false;
    }

    // ������
    bool CheckOpenFour(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = true, rightOpen = true;
            
            // ������
            for (int i = 1; i <= 4; i++) {
                int nx = x - i * dx;
                int ny = y - i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    leftOpen = false;
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] != ChessBoardFlag) {
                    leftOpen = false;
                    break;
                } else {
                    break;
                }
            }
            
            // ���Ҽ��
            for (int i = 1; i <= 4; i++) {
                int nx = x + i * dx;
                int ny = y + i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    rightOpen = false;
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] != ChessBoardFlag) {
                    rightOpen = false;
                    break;
                } else {
                    break;
                }
            }
            
            if (count == 4 && leftOpen && rightOpen) {
                return true;
            }
        }
        
        return false;
    }

    // ������
    bool CheckHalfOpenFour(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = false, rightOpen = false;
            
            // ������
            for (int i = 1; i <= 4; i++) {
                int nx = x - i * dx;
                int ny = y - i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] == ChessBoardFlag) {
                    leftOpen = true;
                    break;
                } else {
                    break;
                }
            }
            
            // ���Ҽ��
            for (int i = 1; i <= 4; i++) {
                int nx = x + i * dx;
                int ny = y + i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] == ChessBoardFlag) {
                    rightOpen = true;
                    break;
                } else {
                    break;
                }
            }
            
            if (count == 4 && (leftOpen || rightOpen)) {
                return true;
            }
        }
        
        return false;
    }

    // ������
    bool CheckOpenThree(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = true, rightOpen = true;
            
            // ������
            for (int i = 1; i <= 3; i++) {
                int nx = x - i * dx;
                int ny = y - i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    leftOpen = false;
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] != ChessBoardFlag) {
                    leftOpen = false;
                    break;
                } else {
                    break;
                }
            }
            
            // ���Ҽ��
            for (int i = 1; i <= 3; i++) {
                int nx = x + i * dx;
                int ny = y + i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    rightOpen = false;
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] != ChessBoardFlag) {
                    rightOpen = false;
                    break;
                } else {
                    break;
                }
            }
            
            if (count == 3 && leftOpen && rightOpen) {
                return true;
            }
        }
        
        return false;
    }

    // ���˫����
    bool CheckDoubleOpenThree(int x, int y, char flag) {
        ChessBoard[x][y] = flag;
        int openThreeCount = 0;
        
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = true, rightOpen = true;
            
            // ������
            for (int i = 1; i <= 3; i++) {
                int nx = x - i * dx;
                int ny = y - i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    leftOpen = false;
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] != ChessBoardFlag) {
                    leftOpen = false;
                    break;
                } else {
                    break;
                }
            }
            
            // ���Ҽ��
            for (int i = 1; i <= 3; i++) {
                int nx = x + i * dx;
                int ny = y + i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N) {
                    rightOpen = false;
                    break;
                }
                
                if (ChessBoard[nx][ny] == flag) {
                    count++;
                } else if (ChessBoard[nx][ny] != ChessBoardFlag) {
                    rightOpen = false;
                    break;
                } else {
                    break;
                }
            }
            
            if (count == 3 && leftOpen && rightOpen) {
                openThreeCount++;
            }
        }
        
        ChessBoard[x][y] = ChessBoardFlag;
        return openThreeCount >= 2;
    }

    // �������ֵ����Ӧ��
    int EvaluateBestResponse(char opponent, char self) {
        int bestResponseScore = INT_MIN;
        
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (ChessBoard[i][j] == ChessBoardFlag && HasNeighbor(i, j, 2)) {
                    int score = EvaluatePosition(i, j, opponent, self);
                    if (score > bestResponseScore) {
                        bestResponseScore = score;
                    }
                }
            }
        }
        
        return bestResponseScore;
    }

    // ���λ���Ƿ����ھ�
    bool HasNeighbor(int x, int y, int distance) {
        if (step_count < 4) return true; // ����ʱ����Ҫ�ھ�
        
        for (int dx = -distance; dx <= distance; dx++) {
            for (int dy = -distance; dy <= distance; dy++) {
                if (dx == 0 && dy == 0) continue;
                
                int nx = x + dx;
                int ny = y + dy;
                
                if (nx >= 1 && nx <= N && ny >= 1 && ny <= N) {
                    if (ChessBoard[nx][ny] != ChessBoardFlag) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    int GetVictory(ChessCoordi Pos, char flag) {
        if (CheckWin(Pos.x, Pos.y, flag)) {
            return 1;
        }
        
        // ��������Ƿ�����
        for (size_t i = 1; i < N + 1; ++i) {
            for (size_t j = 1; j < N + 1; ++j) {
                if (ChessBoard[i][j] == ChessBoardFlag)
                    return 0;
            }
        }
        return -1;
    }

    bool GetWiner(ChessCoordi& Pos, int player, char flag) {
        int n = GetVictory(Pos, flag);
        if (n != 0) {
            PrintChessBoard();
            if (n == 1) {
                if (player == 0) {
                    cout << "���Ի�ʤ" << endl;
                } else {
                    cout << "���" << player << "��ʤ" << endl;
                }
            } else if (n == -1) {
                cout << "����" << endl;
            }
            return true;
        }
        return false;
    }

    bool JudgePos(const ChessCoordi& Pos) {
        if (Pos.x >= 1 && Pos.x <= N && Pos.y >= 1 && Pos.y <= N && 
            ChessBoard[Pos.x][Pos.y] == ChessBoardFlag)
            return true;
        return false;
    }

private:
    char ChessBoard[N + 1][N + 1];
    int step_count = 0;
    HANDLE hConsole;
    ChessCoordi lastComputerMove;
    bool gameOver;
};

int main() {
    srand((unsigned int)time(NULL));
    FiveChess a;
    a.InitChessBoard();
    a.PrintChessBoard();
    a.Play();
    system("pause");
    return 0;
}
