#pragma once
#include <phonon.h>
#include <vector>
#include "SteamAudio.hpp"

/// Interface to set up and send data to/from Steam Audio from the rest of the engine.
class SteamAudioInterface {
    public:

        /// Create default.
        SteamAudioInterface();

        /// Destructor.
        ~SteamAudioInterface();

        /// Sets the main context used by Steam Audio.
        /**
         * @param logCallback Callback for logging messages. Can be NULL. Default uses stdout.
         * @param allocCallback Callback for allocating memory. Can be NULL. Default uses malloc.
         * @param freeCallback Callback for freeing memory. Can be NULL. Default uses free.
         */
        void SetContext(IPLLogFunction logCallback, IPLAllocateFunction allocCallback, IPLFreeFunction freeCallback);
    
        /// Creates a scene object, to be populated with meshes.
        /**
         * @param settings The settings to use for simulation.
         * @param numMaterials Number of different materials used. May not be added or removed after Scene is created.
         * @return Scene object to be used for adding meshes.
         */
        IPLhandle* CreateScene(IPLSimulationSettings settings, int numMaterials);

        /// Destroys a scene object (once reference count reaches zero).
        /**
         * @param scene Handle to a scene to be destroyed.
         */
        void DestroyScene(IPLhandle* scene);

        /// Finalizes a scene object.
        /**
         * @param scene Handle to the scene to be finalized.
         * @param progressCallback Callback to check the progress of the finalization. Can be NULL.
         */
        void FinalizeScene(IPLhandle* scene, IPLFinalizeSceneProgressCallback progressCallback); //Can also create Environment (until we start using multiple scenes)
    
        /// Everything needed to load a previously finalized scene stored in a byte array.
        struct SaveData {
            IPLSimulationSettings settings;
            int sceneSize;
            char* scene;
        };

        /// Saves a finalized scene to a struct.
        /**
         * @param scene The scene to save
         * @return A struct containing everything needed to reconstruct the scene with LoadFinalizedScene().
         */
        SaveData SaveFinalizedScene(IPLhandle* scene); //Note to self: Should call iplSaveFinalizedScene with null data pointer first to figure out how big to make the data pointer for a second call

        /// Loads a finalized scene from a struct.
        /**
         * @param data Struct containing all data relevant to reconstructing a scene
         * @return Scene
         */
        IPLhandle* LoadFinalizedScene(SaveData data);

        /// Specifies a single material used by a scene
        /**
         * @param matIndex Index of the material to set. Between 0 and N-1 where N is the value of numMaterials passed to CreateScene().
         * @param material The material properties to set.
         * @param scene The scene to set the material for. If null, will use most recently created scene.
         **/
        void SetSceneMaterial(int matIndex, IPLMaterial material, IPLhandle* scene = nullptr);

        /// Creates a static mesh and adds it to a non-finalized scene.
        /**
         * @param scene Scene to add mesh to.
         * @param verices Vector of all vertices of the mesh.
         * @param indices Vector of all indices of the mesh.
         * @param materialIndex Index of the material to be used. Previously specified in SetSceneMaterial.
         * @return A handle to the static mesh (automatically added to the scene).
         */
        IPLhandle* CreateStaticMesh(IPLhandle* scene, std::vector<IPLVector3> vertices, std::vector<IPLVector3> indices, int materialIndex);

        /// Creates an Environment object for use by the audio engine internally.
        /**
         * @param scene Scene to create Environment from, can be NULL to use direct sound only.
         * @param settings Settings to use for simulation. Must be the same settings used when created the scene!
         */
        void CreateEnvironment(IPLhandle* scene, IPLSimulationSettings settings); //Should save the environment in the class

        /// Give pointers to player positional data.
        /**
         * @param playerPos Player position.
         * @param playerDir Player forward.
         * @param playerUp Player up.
         **/
        void SetPlayer(IPLVector3* playerPos, IPLVector3* playerDir, IPLVector3* playerUp);

        /// Processes audio samples
        /**
         * @param input The audio buffer to be processed.
         * @param sourcePosition The position of the sound source.
         * @param sourceRadius The radius of the source. To determine how much of the source is occluded rather than have it be on/off.
         **/
        void Process(IPLAudioBuffer input, IPLVector3* sourcePosition, float sourceRadius); // Needs to be called in a way so that there's always at least one processed audio frame ready to go.

    private:

        IPLContext* context;

        IPLRenderingSettings renderSettings;

        SteamAudio sAudio;

        std::vector<IPLhandle*> scenes;

        IPLVector3* playerPos;
        IPLVector3* playerDir;
        IPLVector3* playerUp;
};
