# Pack PNGs from resources/icon/icon_{N}.png into a multi-resolution .ico
# Uses System.Drawing to load each PNG and write a single multi-image ICO.
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$sizes = 16, 24, 32, 48, 64, 128, 256
$iconDir = "C:\Users\ideiudicibus\projects\Samplexpress-VST-Plugin\resources\icon"
$out = Join-Path $iconDir "icon.ico"

# Load each PNG into a Bitmap and keep them in memory
$bitmaps = @()
foreach ($s in $sizes) {
    $pngPath = Join-Path $iconDir ("icon_{0}.png" -f $s)
    if (-not (Test-Path $pngPath)) { throw "Missing $pngPath" }
    $bmp = [System.Drawing.Image]::FromFile($pngPath)
    # Resize to exact target if necessary (Edge should already produce exact size)
    if ($bmp.Width -ne $s -or $bmp.Height -ne $s) {
        $resized = New-Object System.Drawing.Bitmap $s, $s
        $g = [System.Drawing.Graphics]::FromImage($resized)
        $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $g.DrawImage($bmp, 0, 0, $s, $s)
        $g.Dispose()
        $bmp.Dispose()
        $bmp = $resized
    }
    $bitmaps += $bmp
    Write-Host ("Loaded {0}x{1}" -f $bmp.Width, $bmp.Height)
}

# Build ICO file in memory
# Header: ICONDIR (6 bytes) + ICONDIRENTRY (16 bytes per image)
# Then image data, one stream of bytes per image (PNG-encoded)

$ms = New-Object System.IO.MemoryStream
$bw = New-Object System.IO.BinaryWriter $ms

# ICONDIR
$bw.Write([uint16]0)            # Reserved
$bw.Write([uint16]1)            # Type 1 = ICO
$bw.Write([uint16]$sizes.Count) # Number of images

# We need to know offsets, so first serialize each PNG to a buffer
$pngBuffers = @()
foreach ($bmp in $bitmaps) {
    $pms = New-Object System.IO.MemoryStream
    $bmp.Save($pms, [System.Drawing.Imaging.ImageFormat]::Png)
    $pngBuffers += , $pms.ToArray()
}

# Compute data offset: 6 + 16 * count
$dataOffset = 6 + 16 * $sizes.Count

# Write ICONDIRENTRYs
for ($i = 0; $i -lt $sizes.Count; $i++) {
    $s = $sizes[$i]
    $len = $pngBuffers[$i].Length
    # Width/Height: 0 means 256
    $w = if ($s -ge 256) { 0 } else { $s }
    $h = if ($s -ge 256) { 0 } else { $s }
    $bw.Write([byte]$w)
    $bw.Write([byte]$h)
    $bw.Write([byte]0)     # Colour palette (0 = no palette)
    $bw.Write([byte]0)     # Reserved
    $bw.Write([uint16]1)   # Colour planes
    $bw.Write([uint16]32)  # Bits per pixel
    $bw.Write([uint32]$len)            # Image data size
    $bw.Write([uint32]$dataOffset)     # Offset from start of file
    $dataOffset += $len
}

# Write image data
foreach ($buf in $pngBuffers) {
    $bw.Write($buf)
}

$bw.Flush()
[byte[]]$icoBytes = $ms.ToArray()
$ms.Close()

# Dispose bitmaps
foreach ($bmp in $bitmaps) { $bmp.Dispose() }

[System.IO.File]::WriteAllBytes($out, $icoBytes)
Write-Host ("Wrote {0} ({1} bytes)" -f $out, $icoBytes.Length)
