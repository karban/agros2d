// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#ifndef SOLUTIONSTORE_H
#define SOLUTIONSTORE_H

#include "solutiontypes.h"

class SolutionStore
{
public:
    SolutionArray<double> solution(FieldSolutionID solutionID, int component) const;
    bool contains(FieldSolutionID solutionID) const;
    MultiSolutionArray<double> multiSolution(FieldSolutionID solutionID) const;
    MultiSolutionArray<double> multiSolution(BlockSolutionID solutionID) const;

    // returns MultiSolution with components related to last time step, in which was each respective field calculated
    // this time step can be different for respective fields due to time step skipping
    // intented to be used as initial condition for the newton method
    MultiSolutionArray<double> multiSolutionPreviousCalculatedTS(BlockSolutionID solutionID);

    void addSolution(BlockSolutionID solutionID, MultiSolutionArray<double> multiSolution);

    // removes all solutions with the given time step
    void removeTimeStep(int timeStep);

    int lastTimeStep(FieldInfo* fieldInfo, SolutionMode solutionType) const;
    int lastTimeStep(Block* block, SolutionMode solutionType) const;

    // finds nearest smaller(or equal) time step, where this fieldInfo was calculated
    int nearestTimeStep(FieldInfo* fieldInfo, int timeStep) const;

    double lastTime(FieldInfo* fieldInfo);
    double lastTime(Block* block);

    // last adaptive step for given time step. If time step not given, last time step used implicitly
    int lastAdaptiveStep(FieldInfo* fieldInfo, SolutionMode solutionType, int timeStep = -1);
    int lastAdaptiveStep(Block* block, SolutionMode solutionType, int timeStep = -1);

    QList<double> timeLevels(FieldInfo* fieldInfo);

    // number of time steps, where this fieldInfo was calculated up to this time
    int timeLevelIndex(FieldInfo* fieldInfo, double time);
    double timeLevel(FieldInfo* fieldInfo, int timeLevelIndex);

    FieldSolutionID lastTimeAndAdaptiveSolution(FieldInfo* fieldInfo, SolutionMode solutionType);
    BlockSolutionID lastTimeAndAdaptiveSolution(Block* block, SolutionMode solutionType);

    void clearAll();    

private:
    QList<FieldSolutionID> m_multiSolutions;
    QMap<FieldSolutionID, MultiSolutionArray<double> > m_multiSolutionCache;

    void addSolution(FieldSolutionID solutionID, MultiSolutionArray<double> multiSolution);
    void removeSolution(FieldSolutionID solutionID);
    void removeSolution(BlockSolutionID solutionID);

    void insertMultiSolutionToCache(FieldSolutionID solutionID, MultiSolutionArray<double> multiSolution);

    QString baseStoreFileName(FieldSolutionID solutionID);

    struct StructAdaptivityStep
    {
        StructAdaptivityStep() : StructAdaptivityStep(-1) {}
        StructAdaptivityStep(int st) : step(st), referencePresent(0), normalPresent(0) {}

        QString generate();

        int step;
        bool referencePresent;
        bool normalPresent;
    };

    struct StructField
    {
        StructField() : fieldInfo(NULL) {}
        StructField(FieldInfo* fi) : fieldInfo(fi) {}

        QString generate();

        FieldInfo* fieldInfo;
        QMap<int, StructAdaptivityStep> adaptivitySteps;
    };

    struct StructTimeLevel
    {
        StructTimeLevel() : level(-1), time(0) {}
        StructTimeLevel(int lev, double t) : level(lev), time(t) {}

        QString generate();

        int level;
        double time;
        QMap<FieldInfo*, StructField> fields;
    };

    QMap<int, StructTimeLevel> structure;
};

#endif // SOLUTIONSTORE_H
