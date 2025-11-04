#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <tuple>

/**
 * CFLR.cpp
 * --------------------------------------
 * 实现 CFL-Reachability (CFL 可达性分析) 算法。
 * 算法输入：图（节点 + 带标签的边）
 * 算法目标：在给定上下文无关文法规则下，求解符合文法约束的可达关系。
 * --------------------------------------
 */

using namespace std;

/** 
 * 图的边定义 
 * 每条边为 (源节点, 目标节点, 标签)
 */
struct Edge {
    int src;
    int dst;
    string label;
    Edge(int s, int d, const string& l) : src(s), dst(d), label(l) {}
};

/**
 * CFLR 类：封装图与 CFLR 算法逻辑
 */
class CFLR {
private:
    unordered_map<string, vector<pair<int, int>>> labeledEdges; // label -> [(src, dst)]
    unordered_set<string> edgeSet; // 存储边的唯一字符串形式 "src,label,dst" 用于判重
    vector<Edge> allEdges;         // 保存所有边，便于调试与遍历
    unordered_map<int, vector<pair<int, string>>> adjList; // src -> [(dst, label)]

public:
    /**
     * 构造函数：可选初始化文件路径
     */
    CFLR(const string& filename = "") {
        if (!filename.empty()) {
            loadGraph(filename);
        }
    }

    /**
     * 添加边：返回值表示是否为“新增边”
     */
    bool addEdge(int src, int dst, const string& label) {
        string key = to_string(src) + "," + label + "," + to_string(dst);
        if (edgeSet.count(key)) {
            return false; // 已存在该边
        }
        edgeSet.insert(key);
        labeledEdges[label].push_back({src, dst});
        allEdges.emplace_back(src, dst, label);
        adjList[src].push_back({dst, label});
        return true;
    }

    /**
     * 从文件加载图数据
     * 文件格式：每行 src dst label
     */
    void loadGraph(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "无法打开图文件: " << filename << endl;
            return;
        }
        string line;
        int src, dst;
        string label;
        while (getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            stringstream ss(line);
            ss >> src >> dst >> label;
            addEdge(src, dst, label);
        }
        fin.close();
        cout << "图加载完成：共 " << allEdges.size() << " 条边" << endl;
    }

    /**
     * 打印图的基本信息
     */
    void printGraphInfo() const {
        cout << "----------------------------------\n";
        cout << "图信息统计：" << endl;
        cout << "节点数（估计）：" << adjList.size() << endl;
        cout << "边数：" << allEdges.size() << endl;
        cout << "标签种类：" << labeledEdges.size() << endl;
        cout << "----------------------------------\n";
    }

    /**
     * 示例：执行 CFLR 算法主逻辑（示意版本）
     * 实际情况可根据题目规则补充文法推理部分
     */
    void runCFLR() {
        cout << "开始执行 CFL-Reachability 分析..." << endl;

        // 简化版逻辑示例：
        // 对每条边进行初始化，逐步扩展可达集。
        queue<Edge> worklist;
        for (const auto& e : allEdges) {
            worklist.push(e);
        }

        int addedCount = 0;
        while (!worklist.empty()) {
            Edge cur = worklist.front();
            worklist.pop();

            // 这里应放入文法匹配与规则扩展逻辑（例如 S → A B）
            // 示例：若 label == "A"，则根据规则扩展可达边
            if (cur.label == "A") {
                // 示例规则：A(x,y) 且 B(y,z) ⇒ S(x,z)
                if (labeledEdges.count("B")) {
                    for (auto& p : labeledEdges["B"]) {
                        if (p.first == cur.dst) {
                            if (addEdge(cur.src, p.second, "S")) {
                                worklist.emplace(cur.src, p.second, "S");
                                addedCount++;
                            }
                        }
                    }
                }
            }
        }

        cout << "CFLR 执行完成，共新增 " << addedCount << " 条推导边" << endl;
    }
};

/**
 * 主函数示例
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "用法: " << argv[0] << " <graph_file>" << endl;
        return 1;
    }

    string graphFile = argv[1];
    CFLR analyzer(graphFile);
    analyzer.printGraphInfo();
    analyzer.runCFLR();

    return 0;
}
