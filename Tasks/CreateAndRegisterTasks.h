#include "../Context/App.h"
#include "../Globals.h"

// Base Tasks
#include "TaskEmpty.h"
#include "TaskBase.h"
#include "TaskTools.h"
#include "TaskPickParticle.h"
#include "TaskEnqueuePS.h"
#include "TaskCreatePsFromImage.h"
#include "TaskMouseAddVelocity.h"

// Lecture Tasks
#include "TaskRecursiveTree.h"
#include "TaskLect02_MyFirstTask_Differentiation.h"

#include "TaskLect03_CarCollision.h"
#include "TaskLect03_Billiard.h"

#include "TaskLect04_AddForce.h"
#include "TaskLect04_CircularMotion_Integration.h"
#include "TaskLect04_Pendula.h"

#include "TaskLect05_Springs.h"

#include "TaskFlocking.h"

#include "TaskLect09_RigidBodies.h"

// Assignment Tasks
#include "TaskAssignment_Morph.h"
#include "TaskAssignment_Fireworks.h"
#include "TaskAssignment_GravityPlanets.h"

#include "TaskCollision.h"
#include "TaskCollisionFloor.h"




// To add new tasks, the following 2 steps are needed:
// 1. Add new tasks here (as '#include "TaskMyTask.h')
// 2. Register new task below in createTasks(){...}


namespace
{

    void createTasks(App* app)
    {
        // Base Tasks
        app->registerTask<TaskEmpty>();
        app->registerTask<TaskBase>();
        app->registerTask<TaskTools>();
        app->registerTask<TaskPickParticle>();
        app->registerTask<TaskEnqueuePS>();
        app->registerTask<TaskCreatePsFromImage>();
        app->registerTask<TaskMouseAddVelocity>();
        app->registerTask<TaskRecursiveTree>();

        // Lecture Tasks

        app->registerTask<TaskLect02_MyFirstTask_Differentiation>();

        app->registerTask<TaskLect03_CarCollision>();
        app->registerTask<TaskLect03_Billiard>();

        app->registerTask<TaskLect04_AddForce>();
        app->registerTask<TaskLect04_CircularMotion_Integration>();
        app->registerTask<TaskLect04_Pendula>();

        app->registerTask<TaskLect05_Springs>();
        
        app->registerTask<TaskLect09_RigidBodies>();


        // Assignment Tasks
        app->registerTask<TaskAssignment_Morph>();
        app->registerTask<TaskAssignment_Fireworks>();
        app->registerTask<TaskAssignment_GravityPlanets>();

        app->registerTask<TaskCollision>();
        app->registerTask<TaskCollisionFloor>();



        // Register your own tasks here...


    }

    void setDefaultTasks(App* app)
    {
        // Called on startup, you might setup the start-up-task-configuration here.
        // Thus, the tasks would not be initialized as 'empty'.
        // Just push the tasks, which you would like to show up on startup.

        // Examples:

        // Setup Tools
        //app->pushDefaultTask<TaskPickParticle>();
        //app->pushDefaultTask<TaskTools>();
        //app->pushDefaultTask<TaskEnqueuePS>();

        // Rasterization
        //app->pushDefaultTask<TaskRasterization>();
        
        // Setup Fluid 2d Grid
        //app->pushDefaultTask<TaskFluid2dGrid>();
        //app->pushDefaultTask<TaskPickParticle>(); // -> task1

        // Setup Springs
        
        // app->pushDefaultTask<TaskPickParticle>();
        // app->pushDefaultTask<TaskLect05_Springs>();
        // app->pushDefaultTask<TaskLect04_AddForce>();
        //app->pushDefaultTask<TaskCollisionFloor>();
        // app->pushDefaultTask<TaskCollision>();
        // app->pushDefaultTask<TaskLect09_RigidBodies>();

        // Setup Pendula
        //app->pushDefaultTask<TaskPickParticle>();
        //app->pushDefaultTask<TaskLect04_Pendula>();

        // Setup Gravity Planets
        //app->pushDefaultTask<TaskAssignment_GravityPlanets>();
        //app->pushDefaultTask<TaskEnqueuePS>();

        // Setup Fireworks
        // app->pushDefaultTask<TaskAssignment_Fireworks>();
        // app->pushDefaultTask<TaskLect04_AddForce>();

        // Setup Billiard
        // app->pushDefaultTask<TaskLect03_Billiard>(); // -> task0
        // app->pushDefaultTask<TaskPickParticle>(); // -> task1

        // Mouse force
        // app->pushDefaultTask<TaskMouseAddVelocity>();
        // app->pushDefaultTask<TaskCreatePsFromImage>();
        
    }
}