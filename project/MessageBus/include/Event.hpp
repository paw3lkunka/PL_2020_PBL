#ifndef EVENT_HPP_
#define EVENT_HPP_

/**
 * @brief Enum containing all possible event types * 
 */
enum class Event : unsigned int
{
    // Most probably error, shouldn't be sent intentionally, DATA: none.
    UNKNOWN_EVENT = 0,

#pragma region DEBUG
    // Debug log, info priority, DATA: Pointer to const char* log.
    DEBUG_INFO_LOG,
    // Debug log, warning priority, DATA: Pointer to const char* log.
    DEBUG_WARNING_LOG,
    // Debug log, error priority, DATA: Pointer to const char* log.
    DEBUG_ERROR_LOG,
#pragma endregion

#pragma region INPUT
    // The key was pressed, DATA: int - GLFW keycode.
    KEY_PRESSED,
    // The key was released, DATA: int - GLFW keycode.
    KEY_RELEASED,
    // The key was held down until it repeated int - GLFW keycode.
    KEY_REPEAT,
    // The mouse button was pressed, DATA: int - GLFW mouse button.
    MOUSE_BUTTON_PRESSED,
    // The mouse button was released, DATA: int - GLFW mouse button.
    MOUSE_BUTTON_RELEASED,
    // The mouse cursor was moved, DATA: CursorData {double xPos, double yPos, double xDelta, double yDelta}
    MOUSE_CURSOR_MOVED,
    // The mouse cursor was stopped, DATA: none
    MOUSE_CURSOR_STOPPED,
    // The mouse scroll was moved, DATA: ScrollData {double offset, int axis VERTICAL_SCROLL || HORIZONTAL_SCROLL}
    MOUSE_SCROLL_MOVED,
    // The gamepad was connected, DATA: int - controller id: GLFW joystick number.
    GAMEPAD_CONNECTED,
    // The gamepad was connected, DATA: int - controller id: GLFW joystick number.
    GAMEPAD_DISCONNECTED,
    // The gamepad button was pressed, DATA: GamepadButtonData {int gamepadId - GLFW joystick, int buttonId - GLFW gamepad button}.
    GAMEPAD_BUTTON_PRESSED,
    // The gamepad button was released, DATA: GamepadButtonData {int gamepadId - GLFW joystick, int buttonId - GLFW gamepad button}.
    GAMEPAD_BUTTON_RELEASED,
    // The gamepad axis pressure amount was changed, DATA: GamepadAxisData {int gamepadId - GLFW joystick, int axisId - GLFW gamepad button, float amount}.
    GAMEPAD_AXIS_CHANGED,
#pragma endregion

#pragma region AUDIO
    // Audio Listener, initialization, generating context on device and checking if it is current context, DATA: AudioListener* .
    AUDIO_LISTENER_INIT,
    // Audio Listener, update attributes on device level, DATA: AudioListener* containing dirty flags and new attributes' values.
    AUDIO_LISTENER_UPDATE,
    // Audio Source, update listeners, generating sources for contexts on device, DATA: AudioSource* .
    AUDIO_SOURCE_UPDATE_LISTENERS,
    // Audio Source, update attributes on device level, DATA: AudioSource* containing dirty flags and new attributes' values.
    AUDIO_SOURCE_UPDATE_ATTRIBUTES,
    // Audio Source, play Source's buffer queue starting where offset is, DATA: AudioSource* .
    AUDIO_SOURCE_PLAY,
    // Audio Source, stop playing, stop Source's offset where it is in buffer queued, DATA: AudioSource* .
    AUDIO_SOURCE_STOP,
    // Audio Source, pause Source's offset where it is in buffer queue, DATA: AudioSource* .
    AUDIO_SOURCE_PAUSE,
    // Audio Source, stop playing,set Source's offset on the beginning of buffer queue, DATA: AudioSource* .
    AUDIO_SOURCE_REWIND,
#pragma endregion

#pragma region RENDERER
    // Send mesh renderer data, DATA: Pointer to mesh renderer
    RENDERER_ADD_TO_QUEUE,
    // Send view matrix to renderer, DATA: Pointer to view matrix
    RENDERER_SET_VIEW_MATRIX,
    // Send projection matrix to renderer, DATA: Pointer to projection matrix
    RENDERER_SET_PROJECTION_MATRIX,
#pragma endregion

#pragma region RESOURCES
    //Resource module load file event. DATA: FileSystemData {const char* filepath, FileType typeOfFile}
    LOAD_FILE,

    // Query FROM Renderer to Resource Module. DATA: cont char* filepath;
    QUERY_SHADER_DATA,
    // Shader data to send to Renderer module. DATA: const char* shaderData;
    RECEIVE_SHADER_DATA,

    // Query FROM Renderer to Resource Module. DATA: const char* filepath;
    QUERY_TEXTURE_DATA,
    // Texture data to send to renderer module. DATA: TextureData* {int width, height, nrComponents, unsigned char* data}
    RECEIVE_TEXTURE_DATA,

    // Query FROM Audio Module to Resource Module. DATA: const char* filepath;
    QUERY_AUDIO_DATA,
    // Audio data to send to Audio module. DATA: AudioFile* data;
    RECEIVE_AUDIO_DATA,

    // Query FROM Renderer to Resource Module. DATA: const char* filepath/mesh;
    QUERY_MESH_DATA,
    // Mesh data to send to Renderer module. DATA: Mesh* data;
    RECEIVE_MESH_DATA,

    SETUP_BONES,
#pragma endregion

    // used to define ranges of values
    // ! WARNING ! Must always be at the end of enum !
#pragma region MARKERS
    DEBUG_FIRST = DEBUG_INFO_LOG,
    DEBUG_LAST = DEBUG_ERROR_LOG,
    INPUT_FIRST = KEY_PRESSED,
    INPUT_LAST = GAMEPAD_AXIS_CHANGED,
    AUDIO_FIRST = AUDIO_LISTENER_INIT,
    AUDIO_LAST = AUDIO_SOURCE_REWIND
#pragma endregion

};

#endif /* !EVENT_HPP_ */
