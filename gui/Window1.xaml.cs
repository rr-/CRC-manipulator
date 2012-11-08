/*
 * Created by SharpDevelop.
 * User: rr-
 * Date: 2012-09-21
 * Time: 22:25
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Text.RegularExpressions;
using Microsoft.Win32;

namespace crcmanip_gui
{
	/// <summary>
	/// Interaction logic for Window1.xaml
	/// </summary>
	public partial class Window1 : Window
	{
		public Window1()
		{
			InitializeComponent();
		}
		
		void ProcessButton_Click(object sender, RoutedEventArgs e)
		{
            String processPath = System.Reflection.Assembly.GetEntryAssembly().Location;
            processPath = System.IO.Path.GetDirectoryName(processPath);
            processPath = System.IO.Path.Combine(processPath, "crcmanip.exe");
            if (!System.IO.File.Exists(processPath)) {
            	MessageBox.Show(String.Format("Can't find \"{0:s}\" executable file.", processPath));
            	return;
            }
            
			String inputFile = InputFileTextBox.Text;
			String outputFile = OutputFileTextBox.Text;
			String checksum = ChecksumTextBox.Text;
			if (!System.IO.File.Exists(inputFile)) {
				//MessageBox.Show(String.Format("Input file \"{0:s}\" does not exist.", inputFile));
				InputFileTextBox.Focus();
				InputFileTextBox.FadeOut();
				InputFileTextBox.FadeIn();
				return;
			}
			if (outputFile == "") {
				OutputFileTextBox.Focus();
				OutputFileTextBox.FadeOut();
				OutputFileTextBox.FadeIn();
				return;
			}
			if (outputFile == inputFile) {
				MessageBox.Show("Input and output files cannot be same.");
				OutputFileTextBox.Focus();
				OutputFileTextBox.FadeOut();
				OutputFileTextBox.FadeIn();
				return;
			}
			if (!Regex.IsMatch(checksum, "^[a-fA-F0-9]{8}$")) {
				MessageBox.Show("Invalid checksum format (must be 8-character-long hexadecimal string).");
				ChecksumTextBox.Focus();
				ChecksumTextBox.FadeOut();
				ChecksumTextBox.FadeIn();
				return;
			}
			
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            p.StartInfo.FileName = processPath;
            p.StartInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(processPath);
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.RedirectStandardInput = false;
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = false;
            p.StartInfo.Arguments = String.Format("\"{0}\" \"{1}\" {2}", inputFile, outputFile, checksum);
            p.Start();
            p.WaitForExit();

            if (p.ExitCode == 0) {
            	MessageBox.Show("Patch successful");
            } else {
            	MessageBox.Show(String.Format("There were errors.\nStandard output:\n{0}\n\nStandard error:\n{1}", p.StandardOutput.ReadToEnd(), p.StandardError.ReadToEnd()));
            }
		}
		
		void InputFileButton_Click(object sender, RoutedEventArgs e)
		{
			OpenFileDialog dialog = new OpenFileDialog();
			dialog.Filter = "All files (*.*)|*.*";
			dialog.Title = "Select input file";
			if (dialog.ShowDialog() == true) {
				InputFileTextBox.Text = dialog.FileName;
			}
		}
		
		
		void OutputFileButton_Click(object sender, RoutedEventArgs e)
		{
			SaveFileDialog dialog = new SaveFileDialog();
			dialog.Filter = "All files (*.*)|*.*";
			dialog.Title = "Select output file";
			if (dialog.ShowDialog() == true) {
				OutputFileTextBox.Text = dialog.FileName;
			}
		}
	}
}