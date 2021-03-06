#pragma once

#include <phonon.h>
#include <vector>
#include <cstdint>
#include "SteamAudio.hpp"

namespace Audio {
    class SteamAudioRenderers;

    /// Interface to set up and send data to/from Steam Audio from the rest of the engine.
    class SteamAudioInterface {
        public:
            /// Everything needed to load a previously finalized scene stored in a byte array.
            struct SaveData {
                /// Settings.
                IPLSimulationSettings settings;
                /// Size of scene.
                int sceneSize;
                /// Binary scene pointer.
                IPLbyte* scene;
            };

            /// Constructor.
            SteamAudioInterface();

            /// Destructor.
            ~SteamAudioInterface();

            /// Creates the scene object, to be populated with meshes.
            /**
             * @param numMaterials Number of different materials used. May not be added or removed after Scene is created.
             */
            void CreateScene(uint32_t numMaterials);

            /// Finalizes the scene object.
            /**
             * @param progressCallback Callback to check the progress of the finalization. Can be NULL.
             */
            void FinalizeScene(IPLFinalizeSceneProgressCallback progressCallback);

            /// Create a new SteamAudioRenderers,
            /**
             * @param renderers New SteamAudioRenderers to be created.
             */
            void CreateRenderers(SteamAudioRenderers*& renderers);

            /// Saves the finalized scene to a struct.
            /**
             * @return A struct containing everything needed to reconstruct the scene with LoadFinalizedScene().
             */
            SaveData SaveFinalizedScene();

            /// Loads a finalized scene from a struct.
            /**
             * @param data Struct containing all data relevant to reconstructing a scene
             */
            void LoadFinalizedScene(const SaveData& data);

            /// Specifies a single material used by the scene
            /**
             * @param matIndex Index of the material to set. Between 0 and N-1 where N is the value of numMaterials passed to CreateScene().
             * @param material The material properties to set.
             */
            void SetSceneMaterial(uint32_t matIndex, IPLMaterial material);

            /// Creates a static mesh and adds it to the non-finalized scene.
            /**
             * @param verices Vector of all vertices of the mesh.
             * @param indices Vector of all indices of the mesh.
             * @param materialIndex Index of the material to be used. Previously specified in SetSceneMaterial.
             * @return A handle to the static mesh (automatically added to the scene).
             */
            IPLhandle* CreateStaticMesh(std::vector<IPLVector3> vertices, std::vector<IPLTriangle> indices, int materialIndex);

            /// Creates an Environment object for use by the audio engine internally.
            void CreateEnvironment();

            /// Give pointers to player positional data.
            /**
             * @param playerPos Player position.
             * @param playerDir Player forward.
             * @param playerUp Player up.
             */
            void SetPlayer(IPLVector3 playerPos, IPLVector3 playerDir, IPLVector3 playerUp);

            /// Processes audio samples.
            /// Needs to be called in a way so that there's always at least one processed audio frame ready to go.
            /**
             * @param buffers The raw audio data.
             * @param positions The positions of the sound sources.
             * @param radii The radii of the sources. To determine how much of a source is occluded rather than have it be on/off.
             * @param renderers The SteamAudioRenderers for the audio sources.
             * @param output Final mixed raw audio data.
             */
            void Process(const std::vector<float*>& buffers, const std::vector<IPLVector3>& positions, const std::vector<float>& radii, const std::vector<SteamAudioRenderers*>& renderers, float* output);

        private:
            IPLContext context;

            IPLSimulationSettings simSettings;

            SteamAudio sAudio;

            IPLhandle scene = NULL;
            IPLhandle environment = NULL;
    };
}
