#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <tuple>

using v_type = unsigned short;

constexpr int factorial(int x) {
    if (x <= 1) return 1;
    return x * factorial(x - 1);
}

constexpr int double_factorial(int x) {
    if (x <= 1) return 1;
    return x * double_factorial(x - 2);
}

constexpr int npow(int x, unsigned int n) {
    if (n == 0) return 1;
    if (n % 2 == 0) {
        int a = npow(x, n/2);
        return a * a;
    } else {
        return x * npow(x, n-1);
    }
}

/*
 * MODIFY THESE PARAMETERS FOR RUNNING THE PROGRAM
 */
constexpr v_type NUM_EXTERNAL_VERTS = 2;
constexpr v_type NUM_THREE_POINT_VERTS = 2;
constexpr v_type NUM_FOUR_POINT_VERTS = 0;

constexpr int LOG_MOD = 1000000;

/*
 * DERIVED CONSTANTS
 */
constexpr v_type NUM_VERTICES = NUM_EXTERNAL_VERTS + NUM_THREE_POINT_VERTS + NUM_FOUR_POINT_VERTS;
constexpr v_type NUM_FIELDS = NUM_EXTERNAL_VERTS + 3 * NUM_THREE_POINT_VERTS + 4 * NUM_FOUR_POINT_VERTS;
std::array<char, NUM_VERTICES> VERT_NAMES;
constexpr int NORMALIZATION_FACTOR
    = factorial(NUM_THREE_POINT_VERTS + NUM_FOUR_POINT_VERTS)
      * npow(factorial(3), NUM_THREE_POINT_VERTS)
      * npow(factorial(4), NUM_FOUR_POINT_VERTS);
constexpr int NUM_EXPECTED = double_factorial(NUM_FIELDS-1);

std::array<v_type, NUM_FIELDS> create_fields() {
    std::array<v_type, NUM_FIELDS> fields;
    int start = 0;
    for (int i = 0; i < NUM_EXTERNAL_VERTS; i++) {
        fields[i] = i;
    }
    start += NUM_EXTERNAL_VERTS;
    for (int i = 0; i < NUM_THREE_POINT_VERTS; i++) {
        for (int j = 0; j < 3; j++) {
            fields[3 * i + j + start] = NUM_EXTERNAL_VERTS + i;
        }
    }
    start += 3 * NUM_THREE_POINT_VERTS;
    for (int i = 0; i < NUM_FOUR_POINT_VERTS; i++) {
        for (int j = 0; j < 4; j++) {
            fields[4 * i + j + start] = NUM_EXTERNAL_VERTS + NUM_THREE_POINT_VERTS + i;
        }
    }
    return fields;
}

void initialize() {
    int i = 0;
    // External vertices are labelled by numbers
    std::cout << "External vertices: ";
    for ( ; i < NUM_EXTERNAL_VERTS; i++) {
        VERT_NAMES[i] = 49 + i;
        std::cout << VERT_NAMES[i] << " ";
    }
    std::cout << "\n";

    // Internal vertices labelled by letters
    std::cout << "Internal vertices: ";
    for ( ; i < NUM_VERTICES; i++) {
        VERT_NAMES[i] = 97 + i - NUM_EXTERNAL_VERTS;
        std::cout << VERT_NAMES[i] << " ";
    }
    std::cout << "\n";

    std::cout << "Number of 3 point vertices: " << NUM_THREE_POINT_VERTS << "\n"
        << "Number of 4 point vertices: " << NUM_FOUR_POINT_VERTS << "\n"
        << "Number of fields: " << NUM_FIELDS << "\n"
        << "Normalization factor (taylor factor * interaction #s): " << NORMALIZATION_FACTOR << "\n"
        << "Number of expected diagrams: " << NUM_EXPECTED << "\n";

    auto fields = create_fields();
    std::cout << "Fields: [";
    for (int j = 0; j < fields.size(); j++) {
        if (j > 0) std::cout << ", ";
        std::cout << VERT_NAMES[fields[j]];
    }
    std::cout << "]\n";
}

struct FeynmanDiagram {
    // Edge graph
    std::array<std::array<v_type, NUM_VERTICES>, NUM_VERTICES> graph;

    /*
     * Create a diagram in place from a string of fields.
     * Edges are assumed to be consecutive elements in the fields, e.g. by indices
     *   0-1 2-3 4-5 ...
     */
    void create(const std::array<v_type, NUM_FIELDS>& fields) {
        clear();
        
        for (int i = 0; i < NUM_FIELDS; i += 2) {
            add_edge(fields[i], fields[i+1]);
        }
    }

    void clear() {
        for (int i = 0; i < graph.size(); i++) {
            graph[i].fill(0);
        }
    }

    // Check if graphs are identical
    bool operator==(const FeynmanDiagram& diagram) const {
        return graph == diagram.graph;
    }

    // Check if graphs are equal under internal permutations
    bool equal_under_internal_permutations(const FeynmanDiagram& diagram) const {
        std::array<v_type, NUM_VERTICES> verts;
        for (v_type i = 0; i < NUM_VERTICES; i++) verts[i] = i;

        do {
            bool same = true;
            for (int i = 0; i < verts.size() && same; i++) {
                for (int j = i; j < verts.size() && same; j++) {
                    same = (graph[i][j] == diagram.graph[verts[i]][verts[j]]);
                }
            }
            if (same) return true;
        } while(std::next_permutation(verts.begin() + NUM_EXTERNAL_VERTS, verts.end()));
        return false;
    }

    // Edges are undirected so add them symmetrically
    void add_edge(int a, int b) {
        graph[a][b]++;
        if (a != b)
            graph[b][a]++;
    }

    void dump() const {
        for (int i = 0; i < graph.size(); i++) {
            for (int j = 0; j < graph.size(); j++) {
                std::cout << graph[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    void dump_edges() const {
        for (int i = 0; i < graph.size(); i++) {
            for (int j = i; j < graph.size(); j++) {
                if (graph[i][j] > 0) {
                    std::cout << "[" << VERT_NAMES[i] << ", " << VERT_NAMES[j] << "]";
                    if (graph[i][j] > 1) std::cout << "^" << graph[i][j];
                    std::cout << " ";
                }
            }
        }
        std::cout << "\n";
    }
};

class Counter {
public:
    Counter()
        : n(0)
    {
        fields = create_fields();
 
        // If trying to count only one field, use this:
        // diagram.create({0, 2, 1, 3, 2, 3, 2, 3});
        // found_graphs.push_back(std::make_tuple(diagram, 0));
    };

    void compute() {
        count(0);
    }

    // generate pairings of list using recursive permutation algorithm, but skip by 2
    void count(int start) {
        static_assert(NUM_FIELDS % 2 == 0);

        if (start >= NUM_FIELDS) {
            n++;
            if (n % LOG_MOD == 0) std::cout << n << "\n";
            add_field();                // count all fields
            // only_add_field();        // count only one field
        } else {
            for (size_t j = start+1; j < fields.size(); j++) {
                std::swap(fields[j], fields[start+1]);

                count(start+2);

                std::swap(fields[j], fields[start+1]);
            }
        }
    }

    const std::vector<std::tuple<FeynmanDiagram, int>>& get_found_graphs() const { return found_graphs; }

    void consolidate_graphs() {
        std::vector<std::tuple<FeynmanDiagram, int>> final_graphs;
        for (const auto&[graph, n] : found_graphs) {
            bool found = false;
            for (auto&[final_graph, m] : final_graphs) {
                if (graph.equal_under_internal_permutations(final_graph)) {
                    m += n;
                    found = true;
                    break;
                }
            }

            if (!found) {
                final_graphs.push_back(std::make_tuple(graph, n));
            }
        }
        found_graphs = final_graphs;
    }

private:
    FeynmanDiagram diagram;
    std::array<v_type, NUM_FIELDS> fields;
    std::vector<std::tuple<FeynmanDiagram, int>> found_graphs;
    int n;

    void add_field() {
        diagram.create(fields);

        bool found = false;
        for (auto&[graph, n] : found_graphs) {
            if (graph == diagram) {
                n++;
                found = true;
                break;
            }
        }

        if (!found) {
            found_graphs.push_back(std::make_tuple(diagram, 1));
        }
    }

    void only_add_field() {
        diagram.create(fields);

        auto& [target, n] = found_graphs.front();

        if (target.equal_under_internal_permutations(diagram)) {
            n++;
        }
    }
};

int main(void) {

    initialize();

    Counter counter;
    counter.compute();
    counter.consolidate_graphs();

    long int total = 0;
    std::cout << "Number of found graphs: " << counter.get_found_graphs().size() << "\n";
    for (const auto&[graph, n] : counter.get_found_graphs()) {
        std::cout << "Graph Count: " << n << "\n";
        std::cout << "Symmetry Factor: " << (NORMALIZATION_FACTOR / n) << "\n";
        graph.dump_edges();
        std::cout << "\n";
        total += n;
    }
    std::cout << "TOTAL count: " << total << "\n";
    std::cout << "EXPECTED: " << NUM_EXPECTED << "\n";

    // Search for a single graph
    //
    // FeynmanDiagram diagram;
    // diagram.create({0, 1, 0, 2, 1, 3, 2, 3, 2, 2, 1, 1});
    // 
    // for (const auto&[graph, n] : counter.get_found_graphs()) {
    //     if (graph.equal_under_internal_permutations(diagram)) {
    //         std::cout << "Found graph!\n"
    //             << "Graph count: " << n << "\n"
    //             << "Symmetry Factor: " << (prefactor / n) << "\n";
    //         graph.dump_edges();
    //         std::cout << "\n";
    //     }
    // }

    return 0;
}
