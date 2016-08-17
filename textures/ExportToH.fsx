open System
open System.Drawing
open System.Text
open System.IO
open System.Windows.Forms

let args = Environment.GetCommandLineArgs()
let currentDir = __SOURCE_DIRECTORY__

let mutable counter = 0
Directory.GetFiles(currentDir, "*.png")
|> Array.iter (fun file ->  let bitmap = new Bitmap(file)
                            let dataBuilder = new StringBuilder()
                            for y in 0..bitmap.Height-1 do
                                for x in 0..bitmap.Width-1 do
                                    let pixel = bitmap.GetPixel(x, y)
                                    dataBuilder.AppendFormat("\\{0}\\{1}\\{2}\\{3}", 
                                        Convert.ToString(pixel.R, 8),
                                        Convert.ToString(pixel.G, 8),
                                        Convert.ToString(pixel.B, 8),
                                        Convert.ToString(pixel.A, 8))
                                    counter <- counter + 1
                                    if counter >= 50 then 
                                        dataBuilder.Append("\"\r\n\"")
                                        counter <- 0
   
                            File.WriteAllText(String.Concat(Path.GetFileNameWithoutExtension(file), "_data.h"), String.Format("const struct {{ unsigned int width; unsigned int height; unsigned int bytes_per_pixel; unsigned char pixel_data[{0} * {1} * 4 + 1]; }} {2} = {{ {0},{1},4, \"{3}\" }};",
                                        bitmap.Width, bitmap.Height, String.Concat(Path.GetFileNameWithoutExtension(file), "_data"), dataBuilder.ToString()));
           
                            bitmap.Dispose())

MessageBox.Show("Completed")


