#include <vector>
#include <limits>

extern "C" {
#include <Python.h>
}

#include <iostream>


const double INF = std::numeric_limits<double>::infinity();

// Element of the net
struct CircuitElement {
    long net_from;
    long net_to;
    double resistance;
    double reverse_resistance;
};

typedef std::vector<CircuitElement> Schematics;         // input net
typedef std::vector< std::vector<double> > Graph;       // output resistance matrix

//////////////////////////////////////////////////////////////////////////
/// Start Data Convention

static Schematics pyobject_to_cpp( PyObject* py_list )
{
    Schematics result;
    result.resize( PyObject_Length(py_list) );
    for( size_t i = 0; i < result.size(); ++i ) 
    {
        PyObject* py_record = PyList_GetItem( py_list, i );
        if( py_record == nullptr ) {
            std::cout << "py_record == nullptr" << std::endl;
        }

        CircuitElement& record = result[i];

        PyObject* py_net_from = PyList_GetItem( py_record, 0 );
        if( py_net_from == nullptr ) {
            std::cout << "py_net_from == nullptr" << std::endl;
        }
        record.net_from = PyLong_AsLong( py_net_from ) - 1;

        PyObject* py_net_to = PyList_GetItem( py_record, 1 );
        if( py_net_to == nullptr ) {
            std::cout << "py_net_to == nullptr" << std::endl;
        }
        record.net_to = PyLong_AsLong( py_net_to ) - 1;

        PyObject* py_resistance = PyList_GetItem( py_record, 2 );
        if( py_resistance == nullptr ) {
            std::cout << "py_resistance == nullptr" << std::endl;
        }
        record.resistance = PyFloat_AsDouble( py_resistance );

        PyObject* py_reverse_resistance = PyList_GetItem( py_record, 3 );
        if( py_reverse_resistance == nullptr ) {
            std::cout << "py_reverse_resistance == nullptr" << std::endl;
        }
        record.reverse_resistance = PyFloat_AsDouble( py_reverse_resistance );
    }
    return result;
}

static PyObject* cpp_to_pyobject( const std::vector< std::vector<double> >& matrix )
{
    PyObject* result = PyList_New( matrix.size() );
    for( size_t i = 0; i < matrix.size(); ++i ) 
    {
        const std::vector<double>& row = matrix[i];
        PyObject* py_row = PyList_New( row.size() );
        PyList_SetItem( result, i, py_row );
        for( size_t j = 0; j < row.size(); ++j ) 
        {
            const double elem = row[j];
            PyObject* py_elem = PyFloat_FromDouble( elem );
            PyList_SetItem( py_row, j, py_elem );
        }
    }
    return result;
}

/// End Data Convention
//////////////////////////////////////////////////////////////////////////


// Calculate new resistance with parallel connection
static double calcNewResistValue( double first, double second )
{
    if( first == INF ) {
        return second;
    }

    if( second == INF ) {
        return first;
    }
    
    return 1.0 / (1.0 / first + 1.0 / second); 
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


static PyObject* resistance_calc_calculate( PyObject* module, PyObject* args )
{
    PyObject* py_schematics = PyTuple_GetItem( args, 0 );
    Schematics schematics = pyobject_to_cpp( py_schematics );

    Graph result = calculateResistance( schematics );

    PyObject* py_result = cpp_to_pyobject( result );
    return py_result;
}


PyMODINIT_FUNC PyInit_resistance_calc()
{
    static PyMethodDef ModuleMethods[] = {
        { "faster_calculate", resistance_calc_calculate, METH_VARARGS, "Faster resistance calculation" },
        { NULL, NULL, 0, NULL }
    };
    static PyModuleDef ModuleDef = {
        PyModuleDef_HEAD_INIT,
        "resistance_calc",
        "Resistance calculation",
        -1, ModuleMethods, 
        NULL, NULL, NULL, NULL
    };

    PyObject* module = PyModule_Create( &ModuleDef );
    return module;
}