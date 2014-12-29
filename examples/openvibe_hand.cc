// This example will show a hand in a default pose
// and then bend one joint and show the hand again
//
// This is a line-by-line code example

#include <string>
#include <thread>
#include <chrono>

#include <iostream>
#include <cstring>
#include <queue>

#include <unistd.h>
#include <math.h>

#include "osc/osc/OscReceivedElements.h"
#include "osc/osc/OscPacketListener.h"
#include "osc/ip/UdpSocket.h"
#define PORT 7000

# include "opencv2/opencv.hpp"
# include "text_printer.h"
# include "hand_pose.h"
# include "hand_renderer.h"
# include "scene_spec.h"

// Don't forget to mention the libhand namespace
using namespace libhand;
using namespace std;

queue<float> ov_values;

// OpenViBE Listener
class ExamplePacketListener : public osc::OscPacketListener {
protected:
    virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint )
    {
        (void) remoteEndpoint; // suppress unused parameter warning

        try{
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.

            if( std::strcmp( m.AddressPattern(), "/libhand" ) == 0 ){
                float value;
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                args >> value >> osc::EndMessage;
                std::cout << "received '/libhand' message with arguments: " << value << "\n";
                ov_values.push(value);
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
    UdpListeningReceiveSocket s(IpEndpointName(IpEndpointName::ANY_ADDRESS, PORT), &listener);
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
    hand_renderer.Setup(800, 600);

    string file_name;

    if (argc > 1) {
        file_name = argv[1];
    } else {
        cerr << "Usage: " << argv[0] << " <scene_spec>\n";
        exit(1);
    }

    // Process the scene spec file
    SceneSpec scene_spec(file_name);

    // Tell the renderer to load the scene
    hand_renderer.LoadScene(scene_spec);

    FullHandPose hand_pose(scene_spec.num_bones());
    hand_pose.Load("../poses/yandan_avuc.yml", scene_spec);
    hand_renderer.SetHandPose(hand_pose, true);
    hand_renderer.RenderHand();

    // Open a window through OpenCV
    string win_name = "OpenViBE LibHand";
    cv::namedWindow(win_name);

    // We can get an OpenCV matrix from the rendered hand image
    cv::Mat pic = hand_renderer.pixel_buffer_cv();

    //TextPrinter text(pic, 800 - 10, 600 - 30);
    //text.SetRightAlign();
    //text.Print("Processing OpenViBE queue...");

    // Now we're going to change the hand pose and render again
    // The hand pose depends on the number of bones, which is specified
    // by the scene spec file.

    // And tell OpenCV to show the rendered hand
    cv::imshow(win_name, pic);
    cv::waitKey(1);

    std::chrono::time_point<std::chrono::system_clock> last_empty, first_filled;
    first_filled = std::chrono::system_clock::now();
    while (1) {
        if (!ov_values.empty()) {
            first_filled = std::chrono::system_clock::now();
            float value = ov_values.front();
            ov_values.pop();
            hand_pose.bend(15) = (value * M_PI/48);
            hand_renderer.SetHandPose(hand_pose);
            hand_renderer.RenderHand();
        } else {
            // Queue empty
            std::chrono::duration<double> elapsed = first_filled - last_empty;
            if (elapsed.count() < -2.0) {
                hand_pose.bend(15) = 0.0;
                hand_renderer.SetHandPose(hand_pose);
                hand_renderer.RenderHand();
            }
            last_empty = std::chrono::system_clock::now();
        }
        cv::imshow(win_name, pic);
        cv::waitKey(30);
    }

    //ov_thread.join();
  } catch (const std::exception &e) {
    cerr << "Exception: " << e.what() << endl;
  }

  return 0;
}
