open System
open System.Drawing
open System.Text
open System.IO
open System.Windows.Forms

let SCALE = 2
let bitmap = new Bitmap("C:\\Users\\RAB\\Documents\\jezzball3ds\\Jezzball3DS\\textures\\sprites.png")
let scaledBitmap = new Bitmap(bitmap.Width*SCALE, bitmap.Height*SCALE)

let FillPixel(x:int,y:int,color:Color) = 
    for w in 0..SCALE-1 do
        for h in 0..SCALE-1 do
            scaledBitmap.SetPixel((x*SCALE)+w,(y*SCALE)+h,color)
    
for y in 0..bitmap.Height-1 do
    for x in 0..bitmap.Width-1 do
        let pixel = bitmap.GetPixel(x, y)
        FillPixel(x,y,pixel)

scaledBitmap.Save("C:\\Users\\RAB\\Documents\\jezzballwiiu\\Jezzball3DS\\Prueba.png")
scaledBitmap.Dispose()
bitmap.Dispose()

