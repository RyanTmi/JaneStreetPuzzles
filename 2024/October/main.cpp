#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

constexpr array<array<char, 6>, 6> grid = {
    {{'A', 'B', 'B', 'C', 'C', 'C'},
     {'A', 'B', 'B', 'C', 'C', 'C'},
     {'A', 'A', 'B', 'B', 'C', 'C'},
     {'A', 'A', 'B', 'B', 'C', 'C'},
     {'A', 'A', 'A', 'B', 'B', 'C'},
     {'A', 'A', 'A', 'B', 'B', 'C'}}
};

struct Solution
{
    int a;
    int b;
    int c;
};

struct Position
{
    int x;
    int y;

    bool operator==(const Position& other) const = default;
};

using Path  = vector<Position>;
using Paths = pair<Path, Path>;

bool IsValid(int x, int y)
{
    return x >= 0 && x < grid.size() && y >= 0 && y < grid.size();
}

int GetGridValue(int x, int y, const Solution& s)
{
    switch (grid[x][y])
    {
        case 'A': return s.a;
        case 'B': return s.b;
        case 'C': return s.c;
        default:  unreachable();
    }
}

bool FindPath(
    Position              current,
    Position              end,
    const Solution&       s,
    int                   score,
    char                  prevCell,
    vector<vector<bool>>& visited,
    Path&                 path,
    Path&                 resultPath)
{
    static constexpr array dx = {2, 1, -1, -2, -2, -1, 1, 2};
    static constexpr array dy = {1, 2, 2, 1, -1, -2, -2, -1};

    static constexpr int target = 2024;

    if (current == end)
    {
        if (score == target)
        {
            resultPath = path;
            return true;
        }
        return false;
    }

    // Prune paths that cannot reach the target score
    if (score > target)
    {
        return false;
    }

    for (int i = 0; i < dx.size(); ++i)
    {
        const int nx = current.x + dx[i];
        const int ny = current.y + dy[i];

        if (IsValid(nx, ny) && !visited[nx][ny])
        {
            const char currCell  = grid[nx][ny];
            const int  currValue = GetGridValue(nx, ny, s);
            int        newScore  = score;

            // Apply scoring rules
            if (currCell != prevCell)
            {
                newScore *= currValue;
            }
            else
            {
                newScore += currValue;
            }

            if (newScore > target)
            {
                continue;
            }

            visited[nx][ny] = true;
            path.push_back(Position(nx, ny));

            if (FindPath(Position(nx, ny), end, s, newScore, currCell, visited, path, resultPath))
            {
                return true;
            }

            path.pop_back();
            visited[nx][ny] = false;
        }
    }

    return false;
}

bool FindTwoPaths(const Solution& s, Paths& paths)
{
    // First path from a6 to f1
    constexpr Position start1(0, 0);
    constexpr Position end1(5, 5);

    // Second path from a1 to f6
    constexpr Position start2(5, 0);
    constexpr Position end2(0, 5);

    vector visited1(grid.size(), vector(grid.size(), false));
    vector visited2(grid.size(), vector(grid.size(), false));

    Path tempPath1 = {start1};
    Path tempPath2 = {start2};

    visited1[start1.x][start1.y] = true;
    visited2[start2.x][start2.y] = true;

    const int startValue1 = GetGridValue(start1.x, start1.y, s);
    const int startValue2 = GetGridValue(start2.x, start2.y, s);

    const bool path1Found =
        FindPath(start1, end1, s, startValue1, grid[start1.x][start1.y], visited1, tempPath1, paths.first);
    const bool path2Found =
        FindPath(start2, end2, s, startValue2, grid[start2.x][start2.y], visited2, tempPath2, paths.second);

    return path1Found && path2Found;
}

struct Result
{
    Solution s;
    Paths    paths;
};

optional<Result> SolveKnightMoves()
{
    // A + B + C <= 50
    constexpr int minSum = 50;

    for (int a = 1; a < minSum; ++a)
    {
        for (int b = 1; b < minSum; ++b)
        {
            if (a == b)
            {
                continue;
            }
            for (int c = 1; c < minSum; ++c)
            {
                if (a == c || b == c || a + b + c > minSum)
                {
                    continue;
                }

                Solution s(a, b, c);
                Paths    paths;

                print("Finding paths for A={}, B={}, C={}... ", a, b, c);
                fflush(stdout);

                const auto start = chrono::high_resolution_clock::now();

                const auto found = FindTwoPaths(s, paths);

                const auto end      = chrono::high_resolution_clock::now();
                const auto duration = chrono::duration_cast<chrono::seconds>(end - start);

                println("Took {}", duration);
                if (found)
                {
                    return Result(s, paths);
                }
            }
        }
    }

    return nullopt;
}

string FormatPath(const Path& path)
{
    string s;
    s.reserve(path.size() * 3);

    for (const auto& [x, y] : path)
    {
        s.push_back('a' + y);
        s.push_back(to_string(grid.size() - x)[0]);
        s.push_back(',');
    }

    s.pop_back();
    return s;
}

int main()
{
    const auto start = chrono::high_resolution_clock::now();

    const auto res = SolveKnightMoves();

    const auto end      = chrono::high_resolution_clock::now();
    const auto duration = chrono::duration_cast<chrono::seconds>(end - start);

    println("Program duration: {}", duration);

    if (res.has_value())
    {
        println("Path a6 to f1:\n{}", FormatPath(res->paths.first));
        println("Path a1 to f6:\n{}", FormatPath(res->paths.second));
    }
    else
    {
        println("Solution not found...");
    }
}
