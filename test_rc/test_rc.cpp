#include <vector>
#include <iostream>


const double INF = 1000000000000;

struct CircuitElement {
    CircuitElement( long _net_from, long _net_to, double _resistance, double _reverse_resistance )
        : net_from(_net_from), net_to(_net_to), resistance(_resistance), reverse_resistance(_reverse_resistance) {}

    long net_from;
    long net_to;
    double resistance;
    double reverse_resistance;
};

typedef std::vector<CircuitElement> Schematics;
typedef std::vector< std::vector<double> > Graph;

static double calcNewResistValue( double first, double second )
{
    if( first == INF ) {
        return second;
    }

    if( second == INF )
    {
        return first;
    }

    double newValue = 1.0 / (1.0 / first + 1.0 / second);
    return newValue; 
}


static Graph generateGraph( const Schematics& schematics )
{
    size_t NumVertex = 0;
    for( size_t index = 0; index < schematics.size(); ++index )
    {
        if( NumVertex < schematics[index].net_from ) {
            NumVertex = schematics[index].net_from;
        }
        if( NumVertex < schematics[index].net_to ) {
            NumVertex = schematics[index].net_to;
        }        
    }

    Graph result( NumVertex + 1 );
    for( size_t index = 0; index < result.size(); ++index ) {
        result[index].assign( NumVertex + 1, INF );
        result[index][index] = 0;
    }

    for( size_t index = 0; index < schematics.size(); ++index )
    {
        const int& net_from = schematics[index].net_from;
        const int& net_to = schematics[index].net_to;
        result[net_from][net_to] = calcNewResistValue( result[net_from][net_to], schematics[index].resistance );
        result[net_to][net_from] = calcNewResistValue( result[net_to][net_from], schematics[index].reverse_resistance );
    }

    return result;
}

static Graph calculateResistance( const Schematics& schematics )
{
    Graph graph = generateGraph( schematics );
    size_t numVertex = graph.size();

    for( size_t k = 0; k < numVertex; ++k )
    {
        for( size_t i = 0; i < numVertex; ++i )
        {
            for( size_t j = 0; j < numVertex; ++j )
            {
                if( i != j && i != k && j != k && graph[i][k] < INF && graph[k][j] < INF ) {
                    double newPathWeight = graph[i][k] + graph[k][j];
                    graph[i][j] = calcNewResistValue( graph[i][j], newPathWeight );
                }
            }
        }
    }

    return graph;
}


int main()
{
    Schematics schematics;
    schematics.push_back( CircuitElement(2, 0, 1000.000, 1000.000) );
    schematics.push_back( CircuitElement(2, 1, 423074289.097, 423074289.097) );
    schematics.push_back( CircuitElement(0, 2, 84.986, 19295131.171) );
    schematics.push_back( CircuitElement(0, 1, 128.445, 19629496.476) );

    Graph result = calculateResistance( schematics );
    std::cout << "calculateResistance" << std::endl;
    return 0;
}