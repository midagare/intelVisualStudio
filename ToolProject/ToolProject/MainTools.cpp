#include <iostream>
#include <thread>
#include <sstream>
//#include <fstream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>   // Include OpenCV API

void thread_function(int n)
{
	for (int i = 0; i < 10; i++) {
		std::stringstream ss;
		//int r = rand() % 100;
		int r = 12 % 100;
		ss << "Thread " << n << " function Executing:" << i << "; time:" << r;
		for (int j = 0; j < n; j++)
			ss << std::endl;

		std::cout << ss.str();
		//std::this_thread::sleep_for(std::chrono::milliseconds(r));
		//std::cout << "function Executing";
	}
}

void thread_example() {
	std::thread t[3];
	t[0] = std::thread(thread_function, 1);
	t[1] = std::thread(thread_function, 2);
	t[2] = std::thread(thread_function, 3);
	//std::thread threadObj(thread_function, 1);
	//std::thread threadObj2(thread_function, 2);
	//t[2].join();
	for (int i = 0; i < 10; i++) {
		std::stringstream ss;
		ss << "Display From MainThread:" << i << std::endl;
		std::cout << ss.str();
		//std::cout << "MainThread" << std::endl;
	}
	//threadObj.join();
	//threadObj2.join();
	t[0].join();
	t[1].join();
	t[2].join();
	std::cout << "Exit of Main function" << std::endl;
}

void opencv_example() {
	cv::Mat img = cv::imread("D:/Documents/01-Git/github/intelVisualStudio/ToolProject/lena.jpg");
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", img);
	cv::waitKey(0);
}


//TODO: data a tres thread, cada thread guarda en un buffer circular en cada uno de los thread
int opencv_intel_example() try 
{
	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;
	// Start streaming with default recommended configuration
	pipe.start();

	const auto window_name = "Display Image";
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

	while (cv::waitKey(1) < 0 && cvGetWindowHandle(window_name))
	{
		rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		rs2::frame depth = data.get_depth_frame().apply_filter(color_map);

		

		// Query frame size (width and height)
		const int w = depth.as<rs2::video_frame>().get_width();
		const int h = depth.as<rs2::video_frame>().get_height();

		// Create OpenCV matrix of size (w,h) from the colorized depth data
		cv::Mat image(cv::Size(w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);

		// Update the window with new data
		imshow(window_name, image);
	}

	return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
	std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}

int opencv_multicamera() {
	int width = 1280;
	int height = 800;
	int fps = 25;

	const auto window_name_l = "img_l";
	namedWindow(window_name_l, cv::WINDOW_AUTOSIZE);
	//namedWindow(window_name_l, cv::WINDOW_NORMAL);

	const auto window_name_r = "img_r";
	namedWindow(window_name_r, cv::WINDOW_AUTOSIZE);

	const auto window_name_d = "depth";
	namedWindow(window_name_d, cv::WINDOW_AUTOSIZE);

	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	//Create a configuration for configuring the pipeline with a non default profile
	rs2::config cfg;

	//Add desired streams to configuration
	cfg.enable_stream(RS2_STREAM_INFRARED, 1, width, height, RS2_FORMAT_Y16, fps);
	cfg.enable_stream(RS2_STREAM_INFRARED, 2, width, height, RS2_FORMAT_Y16, fps);
	////cfg.enable_stream(RS2_STREAM_COLOR, width, height, RS2_FORMAT_BGR8, fps);
	//cfg.enable_stream(RS2_STREAM_DEPTH, width, height, RS2_FORMAT_Z16, fps);

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;

	//Instruct pipeline to start streaming with the requested configuration
	pipe.start(cfg);

	// Capture 30 frames to give autoexposure, etc. a chance to settle
	for (auto i = 0; i < 30; ++i) pipe.wait_for_frames();

	while (1) // Application still alive?
	{
		// wait for frames and get frameset
		rs2::frameset frameset = pipe.wait_for_frames();
		// get single infrared frame from frameset
		//rs2::video_frame ir_frame = frameset.get_infrared_frame();

		// get left and right infrared frames from frameset
		rs2::video_frame ir_frame_left = frameset.get_infrared_frame(1);
		rs2::video_frame ir_frame_right = frameset.get_infrared_frame(2);

		//rs2::video_frame depth_frame = frameset.get_color_frame().apply_filter(color_map);
		//rs2::frame depth = data.get_depth_frame().apply_filter(color_map);
		//rs2::video_frame depth_frame = color_map(frameset.get_depth_frame());
		//rs2::video_frame depth_frame = frameset.get_depth_frame().apply_filter(color_map); //work
		////rs2::video_frame depth_frame = frameset.get_color_frame();

		cv::Mat dMat_left = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_left.get_data());
		cv::Mat dMat_right = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_right.get_data());
		//cv::Mat dMat_depth = cv::Mat(cv::Size(width, height), CV_8UC3, (void*)depth_frame.get_data());
		////cv::Mat dMat_depth = cv::Mat(cv::Size(width, height), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);

		cv::imshow(window_name_l, dMat_left);
		cv::imshow(window_name_r, dMat_right);
		////cv::imshow(window_name_d, dMat_depth);
		char c = cv::waitKey(1);
		if (c == 's')
		{
			for (int i = 0; i < 10; i++) {
				frameset = pipe.wait_for_frames();
				ir_frame_left = frameset.get_infrared_frame(1);
				ir_frame_right = frameset.get_infrared_frame(2);
				////depth_frame = frameset.get_color_frame();

				dMat_left = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_left.get_data());
				dMat_right = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_right.get_data());
				////dMat_depth = cv::Mat(cv::Size(width, height), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
				cv::imwrite(std::to_string(i) + "-1.jpg", dMat_left);
				cv::imwrite(std::to_string(i) + "-2.jpg", dMat_right);
				////cv::imwrite(std::to_string(i) + "-3.jpg", dMat_depth);
			}
			break;
		}
		else if (c == 'q')
			break;
	}

	/*
	// wait for frames and get frameset
	rs2::frameset frameset = pipe.wait_for_frames();
	// get single infrared frame from frameset
	//rs2::video_frame ir_frame = frameset.get_infrared_frame();

	// get left and right infrared frames from frameset
	rs2::video_frame ir_frame_left = frameset.get_infrared_frame(1);
	rs2::video_frame ir_frame_right = frameset.get_infrared_frame(2);

	cv::Mat dMat_left = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_left.get_data());
	cv::Mat dMat_right = cv::Mat(cv::Size(width, height), CV_8UC1, (void*)ir_frame_right.get_data());

	cv::imshow(window_name_l, dMat_left);
	cv::imshow(window_name_r, dMat_right);

	rs2::video_frame vf = ir_frame_left;
	// Write images to disk
	std::stringstream png_file;
	png_file << window_name_l << vf.get_profile().stream_name() << ".png";
	stbi_write_png(png_file.str().c_str(), vf.get_width(), vf.get_height(),
		vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());
	std::cout << "Saved " << png_file.str() << std::endl;


	vf = ir_frame_right;

	std::stringstream png_file_r;
	png_file_r << window_name_r << vf.get_profile().stream_name() << ".png";
	stbi_write_png(png_file_r.str().c_str(), vf.get_width(), vf.get_height(),
		vf.get_bytes_per_pixel(), vf.get_data(), vf.get_stride_in_bytes());
	std::cout << "Saved " << png_file_r.str() << std::endl;
	*/

	/*
		//Get each frame
		rs2::frame ir_frame = frames.first(RS2_STREAM_INFRARED);
		rs2::frame depth_frame = frames.get_depth_frame();




		rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		rs2::frame depth = color_map(data.get_depth_frame());

		// Query frame size (width and height)
		const int w = depth.as<rs2::video_frame>().get_width();
		const int h = depth.as<rs2::video_frame>().get_height();

		// Create OpenCV matrix of size (w,h) from the colorized depth data
		Mat image(Size(w, h), CV_8UC3, (void*)depth.get_data(), Mat::AUTO_STEP);

		// Update the window with new data
		imshow(window_name, image);
	*/
	return EXIT_SUCCESS;
}

int opencv_camera_new() {
	rs2::colorizer color_map;

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;
	// Start streaming with default recommended configuration
	pipe.start();

	const auto window_name = "Display Image";
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

	while (cv::waitKey(1) < 0 && cvGetWindowHandle(window_name))
	{
		rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		rs2::frame depth = data.get_depth_frame().apply_filter(color_map);

		// Query frame size (width and height)
		const int w = depth.as<rs2::video_frame>().get_width();
		const int h = depth.as<rs2::video_frame>().get_height();

		// Create OpenCV matrix of size (w,h) from the colorized depth data
		cv::Mat image(cv::Size(w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);

		// Update the window with new data
		imshow(window_name, image);
	}
	return EXIT_SUCCESS;
}

int main()
{
	//thread_example();
	//opencv_example();
	//opencv_intel_example();
	//opencv_multicamera();
	//opencv_camera_new();
	//system("pause");
	return 0;
}