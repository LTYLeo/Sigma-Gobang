#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include<windows.h>
#include<limits>
#include<map> // 用于威胁存储

using namespace std;

const int N = 15; // 棋盘大小
const char ChessBoardFlag = ' ';
const char flag1 = 'O'; // 玩家
const char flag2 = 'X'; // AI

// 位置价值表
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

// 威胁类型定义
enum ThreatType {
    NONE = 0,
    SINGLE = 1,     // 单子
    OPEN_TWO = 2,   // 活二
    HALF_OPEN_TWO = 3, // 眠二
    OPEN_THREE = 4, // 活三
    HALF_OPEN_THREE = 5, // 眠三
    OPEN_FOUR = 6,  // 活四
    HALF_OPEN_FOUR = 7, // 冲四
    FIVE = 8        // 五连
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
                if (mode == 1) { // 玩家VS电脑
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
                } else if (mode == 2) { // 玩家VS玩家
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

            cout << "再来一局 y or no" << endl;
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
                        SetConsoleTextAttribute(hConsole, 14); // 黄色
                        cout << " " << ChessBoard[i][j] << " ";
                        SetConsoleTextAttribute(hConsole, 7); // 默认
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
        cout << "请选择 1.玩家VS电脑 2.玩家VS玩家 3.退出" << endl;
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
                cout << "对不起 您的输入有误。。" << endl;
        }
    }

    void PalyerGo(ChessCoordi& Pos, int player, char flag) {
        PrintChessBoard();
        int x = 0;
        int y = 0;
        while (1) {
            cout << "请玩家" << player << "下一步棋" << endl;
            cin >> Pos.x >> Pos.y;
            if (JudgePos(Pos))
                break;
            else
                cout << "玩家输入错误 ! 请重新输入" << endl;
        }
        ChessBoard[Pos.x][Pos.y] = flag;
        step_count++;
    }

    // 高级AI落子函数（带多步预判）
    void ComputerGo(ChessCoordi& Pos, char flag) {
        // 第一步下在中心位置
        if (step_count == 0) {
            Pos.x = N / 2 + 1;
            Pos.y = N / 2 + 1;
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }

        char opponent = (flag == flag1) ? flag2 : flag1;
        
        // 1. 检查是否有立即获胜的机会
        if (FindImmediateWin(Pos, flag)) {
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }
        
        // 2. 检查是否需要立即防守（玩家有活四、冲四或双活三等威胁）
        if (FindCriticalDefensiveMove(Pos, opponent)) {
            ChessBoard[Pos.x][Pos.y] = flag;
            step_count++;
            return;
        }
        
        // 3. 多步预判：评估所有可能位置
        int bestScore = INT_MIN;
        ChessCoordi bestMove;
        vector<ChessCoordi> candidateMoves;
        
        // 获取所有候选位置
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (ChessBoard[i][j] == ChessBoardFlag) {
                    // 只考虑有棋子周围的区域
                    if (HasNeighbor(i, j, 2)) {
                        candidateMoves.push_back({i, j});
                    }
                }
            }
        }
        
        // 如果没有候选位置，随机选择
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
        
        // 评估每个候选位置
        for (const auto& move : candidateMoves) {
            int score = EvaluatePosition(move.x, move.y, flag, opponent);
            
            // 多步预判：考虑对手的应对
            ChessBoard[move.x][move.y] = flag;
            int opponentScore = EvaluateBestResponse(opponent, flag);
            ChessBoard[move.x][move.y] = ChessBoardFlag;
            
            // 最终分数 = 我方得分 - 对手得分 * 防御系数
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

    // 检查是否有立即获胜的机会
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

    // 检查关键防守位置（活四、冲四、双活三等）
    bool FindCriticalDefensiveMove(ChessCoordi& move, char opponent) {
        // 存储所有威胁位置
        map<pair<int, int>, int> threatMap;
        
        // 扫描整个棋盘寻找威胁
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (ChessBoard[i][j] == ChessBoardFlag) {
                    int threatLevel = EvaluateThreatLevel(i, j, opponent);
                    threatMap[{i, j}] = threatLevel;
                }
            }
        }
        
        // 寻找最高级别的威胁
        int maxThreat = 0;
        ChessCoordi bestDefense;
        for (const auto& threat : threatMap) {
            if (threat.second > maxThreat) {
                maxThreat = threat.second;
                bestDefense = {threat.first.first, threat.first.second};
            }
        }
        
        // 如果有高威胁（活四、冲四、双活三），则防守
        if (maxThreat >= 10000) {
            move = bestDefense;
            return true;
        }
        
        return false;
    }

    // 评估威胁级别
    int EvaluateThreatLevel(int x, int y, char opponent) {
        ChessBoard[x][y] = opponent;
        int threatLevel = 0;
        
        // 检查直接获胜威胁
        if (CheckWin(x, y, opponent)) {
            threatLevel = 20000; // 最高级别威胁
            ChessBoard[x][y] = ChessBoardFlag;
            return threatLevel;
        }
        
        // 检查活四威胁
        if (CheckOpenFour(x, y, opponent)) {
            threatLevel = 15000;
        }
        // 检查冲四威胁
        else if (CheckHalfOpenFour(x, y, opponent)) {
            threatLevel = 10000;
        }
        // 检查双活三威胁
        else if (CheckDoubleOpenThree(x, y, opponent)) {
            threatLevel = 8000;
        }
        // 检查活三威胁
        else if (CheckOpenThree(x, y, opponent)) {
            threatLevel = 5000;
        }
        
        ChessBoard[x][y] = ChessBoardFlag;
        return threatLevel;
    }

    // 评估位置价值（考虑攻防）
    int EvaluatePosition(int x, int y, char self, char opponent) {
        int score = positionValue[x][y]; // 位置基础价值
        
        // 临时放置棋子进行评估
        ChessBoard[x][y] = self;
        score += EvaluateLine(x, y, self, opponent);
        
        // 评估对手威胁
        ChessBoard[x][y] = opponent;
        score += EvaluateLine(x, y, opponent, self) * 1.2; // 防守权重更高
        
        ChessBoard[x][y] = ChessBoardFlag;
        
        return score;
    }
    
    // 评估行、列、对角线的价值
    int EvaluateLine(int x, int y, char self, char opponent) {
        int score = 0;
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}}; // 四个方向
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            // 检查棋型
            int value = EvaluatePattern(x, y, dx, dy, self, opponent);
            score += value;
        }
        
        return score;
    }
    
    // 评估特定方向上的棋型
    int EvaluatePattern(int x, int y, int dx, int dy, char self, char opponent) {
        int score = 0;
        int selfCount = 1;  // 当前位置已经假设是self
        int openEnds = 0;
        bool leftOpen = true, rightOpen = true;
        
        // 向左检查
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
                break; // 空位
            }
        }
        
        // 向右检查
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
                break; // 空位
            }
        }
        
        // 计算开放端口
        if (leftOpen) openEnds++;
        if (rightOpen) openEnds++;
        
        // 根据连子数和开放端口评分
        switch (selfCount) {
            case 5: // 五连
                score += 100000;
                break;
            case 4:
                if (openEnds == 2) score += 10000; // 活四
                else if (openEnds == 1) score += 1000; // 冲四
                break;
            case 3:
                if (openEnds == 2) score += 1000; // 活三
                else if (openEnds == 1) score += 200; // 眠三
                break;
            case 2:
                if (openEnds == 2) score += 200; // 活二
                else if (openEnds == 1) score += 50; // 眠二
                break;
            case 1:
                if (openEnds == 2) score += 10; // 单子
                break;
        }
        
        return score;
    }

    // 检查是否获胜
    bool CheckWin(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1; // 当前位置
            
            // 正向检查
            for (int i = 1; i <= 4; i++) {
                int nx = x + i * dx;
                int ny = y + i * dy;
                
                if (nx < 1 || nx > N || ny < 1 || ny > N || ChessBoard[nx][ny] != flag) {
                    break;
                }
                count++;
            }
            
            // 反向检查
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

    // 检查活四
    bool CheckOpenFour(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = true, rightOpen = true;
            
            // 向左检查
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
            
            // 向右检查
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

    // 检查冲四
    bool CheckHalfOpenFour(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = false, rightOpen = false;
            
            // 向左检查
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
            
            // 向右检查
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

    // 检查活三
    bool CheckOpenThree(int x, int y, char flag) {
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = true, rightOpen = true;
            
            // 向左检查
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
            
            // 向右检查
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

    // 检查双活三
    bool CheckDoubleOpenThree(int x, int y, char flag) {
        ChessBoard[x][y] = flag;
        int openThreeCount = 0;
        
        int dirs[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
        
        for (int d = 0; d < 4; d++) {
            int dx = dirs[d][0];
            int dy = dirs[d][1];
            
            int count = 1;
            bool leftOpen = true, rightOpen = true;
            
            // 向左检查
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
            
            // 向右检查
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

    // 评估对手的最佳应对
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

    // 检查位置是否有邻居
    bool HasNeighbor(int x, int y, int distance) {
        if (step_count < 4) return true; // 开局时不需要邻居
        
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
        
        // 检查棋盘是否已满
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
                    cout << "电脑获胜" << endl;
                } else {
                    cout << "玩家" << player << "获胜" << endl;
                }
            } else if (n == -1) {
                cout << "和棋" << endl;
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
