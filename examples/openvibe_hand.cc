// This example will show a hand in a default pose
// and then bend one joint and show the hand again
//
// This is a line-by-line code example

#include <string>
#include <thread>

#include <iostream>
#include <cstring>

#include "osc/osc/OscReceivedElements.h"
#include "osc/osc/OscPacketListener.h"
#include "osc/ip/UdpSocket.h"
#define PORT 7000

// We will use OpenCV, so include the standard OpenCV header
# include "opencv2/opencv.hpp"

// This is our little library for showing file dialogs
# include "file_dialog.h"

// We need the HandPose data structure
# include "hand_pose.h"

// ..the HandRenderer class which is used to render a hand
# include "hand_renderer.h"

// ..and SceneSpec which tells us where the hand 3D scene data
// is located on disk, and how the hand 3D object relates to our
// model of joints.
# include "scene_spec.h"

// Don't forget to mention the libhand namespace
using namespace libhand;
using namespace std;

// OpenViBE Listener
class ExamplePacketListener : public osc::OscPacketListener {
protected:
    virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint )
    {
        (void) remoteEndpoint; // suppress unused parameter warning

        try{
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.

            if( std::strcmp( m.AddressPattern(), "/test1" ) == 0 ){
                // example #1 -- argument stream interface
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                float value;
                args >> value >> osc::EndMessage;
                std::cout << "received '/test1' message with arguments: " << value << "\n";
            }

        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }
};

void openvibe_listener(void) {
    ExamplePacketListener listener;
    UdpListeningReceiveSocket s(
            IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
            &listener );
    s.RunUntilSigInt();
}

int main(int argc, char **argv) {
  // Make sure to always catch exceptions around the LibHand code.
  // LibHand uses a "RAII" pattern to provide for a clean shutdown in
  // case of any errors.
  try {
    thread ov_thread(openvibe_listener);

    // Setup the hand renderer
    HandRenderer hand_renderer;
    hand_renderer.Setup();

    string file_name;

    if (argc > 1) {
        file_name = argv[1];
    } else {
        // Ask the user to show the location of the scene spec file
        FileDialog dialog;
        dialog.SetTitle("Please select a scene spec file");
        file_name = dialog.Open();
    }

    // Process the scene spec file
    SceneSpec scene_spec(file_name);

    // Tell the renderer to load the scene
    hand_renderer.LoadScene(scene_spec);

    // Now we render a hand using a default pose
    hand_renderer.RenderHand();

    // Open a window through OpenCV
    string win_name = "OpenViBE LibHand Demo";
    cv::namedWindow(win_name);

    // We can get an OpenCV matrix from the rendered hand image
    cv::Mat pic = hand_renderer.pixel_buffer_cv();

    // And tell OpenCV to show the rendered hand
    cv::imshow(win_name, pic);
    cv::waitKey();

    // Now we're going to change the hand pose and render again
    // The hand pose depends on the number of bones, which is specified
    // by the scene spec file.
    FullHandPose hand_pose(scene_spec.num_bones());

    // We will bend the first joint, joint 0, by PI/2 radians (90 degrees)
    hand_pose.bend(0) += 3.14159 / 2;
    hand_renderer.SetHandPose(hand_pose);

    // Then we will render the hand again and show it to the user.
    hand_renderer.RenderHand();
    cv::imshow(win_name, pic);
    cv::waitKey();

    //ov_thread.join();
  } catch (const std::exception &e) {
    cerr << "Exception: " << e.what() << endl;
  }

  return 0;
}
