/*
 * pots_solver.h
 *
 *  Created on: Aug 15, 2019
 *      Author: andy
 */

#ifndef INCLUDE_CC3D_API_H_
#define INCLUDE_CC3D_API_H_

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
// used by C++ source code
#else
#include "stddef.h"
#include "stdio.h"
#endif

/**
 * Use standard Windows results codes, if not using Windows, define the result
 * type
 */
#ifndef WIN32
/**
 *
 * To test an HRESULT value, use the FAILED and SUCCEEDED macros.

 * The high-order bit in the HRESULT or SCODE indicates whether the
 * return value represents success or failure.
 * If set to 0, SEVERITY_SUCCESS, the value indicates success.
 * If set to 1, SEVERITY_ERROR, it indicates failure.
 *
 * The facility field indicates from bits 26-16 is the system
 * service responsible for the error. The FACILITY_ITF = 4 is used for most status codes
 * returned from interface methods. The actual meaning of the
 * error is defined by the interface. That is, two HRESULTs with
 * exactly the same 32-bit value returned from two different
 * interfaces might have different meanings.
 *
 * The code field from bits 15-0 is the application defined error
 * code.
 */
typedef int32_t CC3D_RESULT;

#else
// using Windows, define the result types same as Windows
typedef HRESULT CC3D_RESULT;
#endif

// If Python is included, assume we are building with Python, and enable
// Python extensions.

#ifdef PY_VERSION
#define CC3D_PYTHON_PLUGIN
#endif


// If a plugin does not use Python, we still need the same binary layout as one that
// does, so define the layout of the objects here.
#ifndef CC3D_PYTHON_PLUGIN

/* Nothing is actually declared to be a PyObject, but every pointer to
 * a Python object can be cast to a PyObject*.  This is inheritance built
 * by hand.  Similarly every pointer to a variable-size Python object can,
 * in addition, be cast to PyVarObject*.
 */
typedef struct _object {
    size_t ob_refcnt;
    void *ob_type;
} PyObject;

/* PyObject_HEAD defines the initial segment of every PyObject. */
#define PyObject_HEAD                   PyObject ob_base;

#endif



typedef struct CC3DCell {

    // if we're enabling python, all objects get the same binary layout as
    // standard python objects
    PyObject_HEAD;

    long volume;
    float targetVolume;
    float lambdaVolume;
    double surface;
    float targetSurface;
    float angle;
    float lambdaSurface;
    double clusterSurface;
    float targetClusterSurface;
    float lambdaClusterSurface;
    unsigned char type;
    unsigned char subtype;
    double xCM,yCM,zCM; // numerator of center of mass expression (components)
    double xCOM,yCOM,zCOM; // numerator of center of mass expression (components)
    double xCOMPrev,yCOMPrev,zCOMPrev; // previous center of mass
    double iXX, iXY, iXZ, iYY, iYZ, iZZ; // tensor of inertia components
    float lX,lY,lZ; //orientation vector components - set by MomentsOfInertia Plugin - read only
    float ecc; // cell eccentricity
    float lambdaVecX,lambdaVecY,lambdaVecZ; // external potential lambda vector components
    unsigned char flag;
    float averageConcentration;
    long id;
    long clusterId;
    double fluctAmpl;
    double lambdaMotility;
    double biasVecX;
    double biasVecY;
    double biasVecZ;
    bool connectivityOn;
} CC3DCell;

/**
 * Inform the plugin of
 */

typedef struct CC3DPlugin {
    // if we're enabling python, all objects get the same binary layout as
    // standard python objects
    PyObject_HEAD;


} CC3DPlugin ;


/**
 * Notify the plugin when a new cell should be created.
 */
typedef CC3DCell* (*CC3DCellCreateHandler)
        (void *userData, unsigned char cellType, long cellId, long clusterId);

/**
 * Ask the plugin for the energy change for a proposed spin flip.
 */
typedef double (*CC3DEnergyChangeHandler)
        (void *userData, unsigned *point, CC3DCell *newCell, CC3DCell *oldCell);

/**
* Notify the plugin that the cell just changed for a given voxel
*/
typedef CC3D_RESULT (*CC3DCellTypeChangeHandler)
       (void *userData, unsigned *voxel, CC3DCell *newCell, CC3DCell *oldCell);


enum CC3D_NEIGHBOORHOOD_TYPE {

};


typedef struct CC3DSimulator {
    // if we're enabling python, all objects get the same binary layout as
    // standard python objects
    PyObject_HEAD;

    /**
     * Return the number of cells (and cell Ids) in the cell inventory
     */
    long (*getCellCount)();

    /**
     * Get a list if cell Ids
     * count: number of cell ids to get
     * ids: caller allocated array where the cell ids will be written to
     */
    CC3D_RESULT (*getCellIds)(unsigned count, unsigned *ids);

    /**
     * Get the size of the lattice
     * dims: caller allocated length 3 array, will write lattice dims here,
     *       dims[0] = x, dims[1] = y, dims[2] = z
     */
    CC3D_RESULT (*getLatticeDims)(unsigned *dims);

    /**
     * get the cell pointers for the given array of voxels.
     *
     * voxels is an array in row-major order, i.e. for the i'th voxel, the
     * x, y, z voxel indices are:
     *
     * x_i = voxels[3 * i + 0];
     * y_i = voxels[3 * i + 1];
     * z_i = voxels[3 * i + 2];
     *
     * To ask for the cell pointer at voxel location (12, 23, 45), we would:
     *
     * unsigned voxels[3] = {12, 23, 45};
     * CellBase *cells[1];
     * getCellsForVoxels(1, voxels, cells);
     */
    CC3D_RESULT (*getCellsForVoxels)(unsigned count, unsigned *voxels,
            CC3DCell **cells);

    /**
     * Get the cell pointers for a rectangular region defined by the startVoxel
     * to the end Voxel. Useful for getting a cell neighborhood.
     */
    CC3D_RESULT (*getCellsForVoxelRegion)(unsigned maxSize, unsigned *startVoxel,
            unsigned *endVoxel, CC3DCell **cells);


    /**
     * Get the size of a neighborhood, this is the required number of
     * voxel locations for the getNeighborhood call
     */
    long (*getNeighborhoodRegionCount)(unsigned neighborhoodType);

    CC3D_RESULT (*get)

    /**
     * Add / remove a cell create handler. The cell create handler lets the plugin
     * intercept cell create events, and lets the plugin return a derived cell data
     * type. Only one cell create handler can be active.
     */
    CC3D_RESULT (*setCellCreateHandler)(void *userData, CC3DCellCreateHandler *handler);
    CC3D_RESULT (*hasCellCreateHandler)();
    CC3D_RESULT (*removeCellCreateHandler)();


    CC3D_RESULT (*addEnergyChangeHandler)(void *userData, CC3DEnergyChangeHandler *handler);
    CC3D_RESULT (*removeEnergyChangeHandler)(void *userData, CC3DEnergyChangeHandler *handler);



    CC3D_RESULT (*addCellTypeChangeHandler)(void *userData, CC3DCellTypeChangeHandler* handler);
    CC3D_RESULT (*hasCellTypeChangeHandler)();
    CC3D_RESULT (*removeCellTypeChangeHandler)(void *userData, CC3DCellTypeChangeHandler *handler);

} CC3DSimulator;


/**
 * The plugin factory is the top-level object that a plugin shared library gives back to the
 * calling code. The plugin factory lets the caller enumerate all of the plugins that the
 * shared library provides.
 */
typedef struct CC3DPluginFactory {
    // if we're enabling python, all objects get the same binary layout as
    // standard python objects
    PyObject_HEAD;

    long (*getPluginCount)();
    CC3D_RESULT (*getPluginIds)(unsigned maxSize, unsigned *result);
    CC3D_RESULT (*getPluginDoc)(unsigned pluginId, const char** result);
    CC3D_RESULT (*getPlugionName)(unsigned plugionId, const char** result);
    CC3D_RESULT (*createPlugin)(unsigned pluginId, CC3DSimulator *simulator, CC3DPlugin **result);

} CC3DPluginFactory;



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CC3D_API_H_ */
