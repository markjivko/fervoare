<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

class image_cl {
        
    /*
     * ### Global variables and constructor
     */

    /**
     * Avaliable conversions and quality array
     *
     * @uses Used by load(), save(), convert()
     * @var array
     */
    public $availableConv = array(
        'gif' => false, // #!important; no quality control for gifs
        'png' => false,
        'jpeg' => 100); // maximum jpeg quality

    /**
     * GDF default font
     * Font List:
     * 1-> width=5 px, height=8 px
     * 2-> width=6 px, height=13 px
     * 3-> width=7 px, height=13 px
     * 4-> width=8 px, height=16 px
     * 5-> width=9 px, height=15 px
     *
     * @uses Used by text()
     * @var int
     */
    public $defaultFont = 4;

    /**
     * Default text color (hexa)
     *
     * @uses Used by text()
     * @var string
     */
    public $defaulttextColor = '#000000';

    /**
     * Default text Background - color (hexa) and transparency (0,opaque-127,transparent)
     *
     * @uses Used by filter(), text()
     * @var array
     */
    public $defaulttextBg = array('#ffffff', 127);

    /*
     * ### Helpers
     */
    protected function _corPix($x0, $y0, $x1, $y1, $x2, $y2, $x3, $y3, $x, $y, $SX, $SY) {
        return $this->_intersectLines(
            (($SY - $y) * $x0 + ($y) * $x3) / $SY, (($SY - $y) * $y0 + $y * $y3) / $SY,
            (($SY - $y) * $x1 + ($y) * $x2) / $SY, (($SY - $y) * $y1 + $y * $y2) / $SY,
            (($SX - $x) * $x0 + ($x) * $x1) / $SX, (($SX - $x) * $y0 + $x * $y1) / $SX,
            (($SX - $x) * $x3 + ($x) * $x2) / $SX, (($SX - $x) * $y3 + $x * $y2) / $SX
        );
    }
    protected function _det($a, $b, $c, $d) {
        return $a * $d - $b * $c;
    }
    protected function _hexToRgb($color) {
        // Stript the #
        $c = str_replace('#', '', $color);

        // 3 or 6 letter format? Make it 6
        if (strlen($c) == 3) {
            $c .= $c;
        }

        // Set the result
        $r ['r'] = hexdec(substr($c, 0, 2));
        $r ['g'] = hexdec(substr($c, 2, 2));
        $r ['b'] = hexdec(substr($c, 4, 2));

        // All done
        return $r;
    }
    protected function _intersectLines($x1, $y1, $x2, $y2, $x3, $y3, $x4, $y4) {
        $d = $this->_det($x1 - $x2, $y1 - $y2, $x3 - $x4, $y3 - $y4);
        if ($d == 0)
            $d = 1;
        $px = $this->_det($this->_det($x1, $y1, $x2, $y2), $x1 - $x2, $this->_det($x3, $y3, $x4, $y4), $x3 - $x4) / $d;
        $py = $this->_det($this->_det($x1, $y1, $x2, $y2), $y1 - $y2, $this->_det($x3, $y3, $x4, $y4), $y3 - $y4) / $d;
        return array($px, $py);
    }

    /*
     * ### Basic methods
     */

    /**
     * Bulge
     * 
     * @example 
     * // Create a bulge in the middle of the image
     * {image}->bulge($image);
     * // Create a small bulge (20%) in the middle of the image
     * {image}->bulge($image,20);
     * // Create a small reverse bulge at 20%
     * {image}->bulge($image,-20);
     * // Set radius to 100px
     * {image}->bulge($image,null,100);
     * // Center the bulge at 200 (X),300 (Y)
     * {image}->bulge($image,null,null,200,300);
     * 
     * @param resource $resource
     * @param int $size -> bulge dimension; -100 to 100
     * @param int $radius -> bulge radius
     * @param int $x -> x position of bulge
     * @param int $y -> y position of bulge
     * @return resource
     */
    public function bulge($resource, $size = null, $radius = null, $x = null, $y = null) {
        // Get the image width and height
        $width = imagesx($resource);
        $height = imagesy($resource);

        // Set a default x and y center of bulge
        $x = is_null($x) ? intval($width / 2) : intval($x);
        $y = is_null($y) ? intval($height / 2) : intval($y);
        if ($x == 0)
            $x = 1;
        if ($y == 0)
            $y = 1;

        // The size must be an integer
        $minWH = min($width, $height);
        $size = is_null($size) ? $minWH / 10 : intval($size);

        // Is it 0? No changes to make
        if ($size == 0)
            return $resource;

        // Use the reverse fish eye?
        $reverse = $size <= 0;

        // The max for the size is 100
        if (abs($size) > 100)
            $size = $reverse ? -100 : 100;

        // Set the radius
        $radius = is_null($radius) ? ($minWH / 2 - 2) : intval($radius);

        // Calculate the W coefficient
        $w = 0.0001 + 0.02 * abs($size / 100);

        // Set this for amplification
        $s = $radius / log($w * $radius + 1, 10);

        // Store the last point created
        $last_point = null;

        // Create a new image
        $canvas = $this->canvas($width, $height);

        // Allocate transparent
        $transparent = imagecolorallocatealpha($canvas, 0, 0, 0, 127);

        // Make the changes pixel by pixel
        for ($i = 0; $i < $width; $i++) {
            for ($j = 0; $j < $height; $j++) {
                // Calculate the radius
                $r = sqrt(pow($x - $i, 2) + pow($y - $j, 2));

                // Inside the bubble?
                if ($r <= $radius) {
                    // Get the new radius
                    $fr = $s * log(1 + $w * $r, 10);

                    // Calculate the angle for the polar coordinates
                    $a = atan2(($i - $x), ($j - $y));

                    // Get the new carthesian X coordinate
                    $_i = intval($x + $fr * sin($a));

                    // Get the new carthesian Y coordonate
                    $_j = intval($y + $fr * cos($a));

                    // Get the current color
                    $color = $reverse ? imagecolorat($resource, $_i, $_j) : imagecolorat($resource, $i, $j);
                    if ($color === false)
                        $color = $transparent;

                    // Use this variable to store the distance to the nearest set point
                    if (is_null($last_point)) {
                        $fr = 1;
                    } else {
                        $fr = intval(sqrt(pow($last_point[0] - $_i, 2) + pow($last_point[1] - $_j, 2))) + 1;
                    }

                    // Save the last point
                    if ($fr > 1) {
                        // Use an ellipse to fill extra space
                        if ($fr > $radius / 4) {
                            $fr = 1;
                        }
                        $reverse
                            ? imagefilledellipse($canvas, $i, $j, $fr, $fr + 1, $color)
                            : imagefilledellipse($canvas, $_i, $_j, $fr, $fr + 1, $color);
                    } else {
                        // No need to complicate things
                        $reverse
                            ? imagesetpixel($canvas, $i, $j, $color)
                            : imagesetpixel($canvas, $_i, $_j, $color);
                    }

                    // Save the last point
                    $last_point = array($_i > $width ? $width : $_i, $_j > $height ? $height : $_j);
                }
                // Set the default
                else {
                    // Leave the pixels untouched
                    imagecopy($canvas, $resource, $i, $j, $i, $j, 1, 1);

                    // Save the last point
                    $last_point = array($i, $j);
                }
            }
        }

        // Return the image
        return $canvas;
    }

    /**
     * Create a blank canvas
     * 
     * @example 
     * // 200 by 300 pixels
     * {image}->canvas(200,300);
     * // 200 by 200 pixels
     * {image}->canvas(200);
     * 
     * @param int $width
     * @param int $height
     * @return resource
     */
    public function canvas($width = null, $height = null) {
        // Stop if the GD extension is not load
        if (!in_array('gd', get_loaded_extensions())) {
            throw new Exception("You must enable the GD extension in order to perform image manipulations.");
        }

        // Prepare the width and height
        if (is_null($width)) {
            throw new Exception("Please specify the image width.");
        } else {
            $width = intval($width);
        }
        if (is_null($height)) {
            $height = $width;
        } else {
            $height = intval($height);
        }

        // Return the
        if (false === $resource = imagecreatetruecolor($width, $height)) {
            throw new Exception("Could not create a new blank canvas.");
        } else {
            imagesavealpha($resource, true);
            $transparent = @imagecolorallocatealpha($resource, 0, 0, 0, 127);
            imagefill($resource, 0, 0, $transparent);
            return $resource;
        }
    }

    /**
     * Converts given image to the specified format
     * 
     * @example
     * # Converts given image to gif, saves it as 'testImage.gif' and deletes 'testImage.jpeg'
     * $this->image->convert('testImage.jpeg','gif');
     * # Loads the file once, returns a resource; 'testImate.jpeg' is deleted
     * $this->image->convert('testImage.jpeg','png',false);
     * # Lowers the saved image quality to 80
     * $this->image->convert('testImage.jpeg','png',null,null,80);
     * 
     * @param string $fileName
     * @param string $to
     * <ul>
     * <li>'png'</li>
     * <li>'gif'</li>
     * <li>'jpeg'</li>
     * </ul>
     * @param boolean $saveToFile #! saves in class call file directory
     * @param boolean $deleteOriginal #! delete source file?
     * @param int $quality #! gifs don't support quality adjustment
     * @return BOOLEAN or resource
     */
    public function convert($fileName, $to, $saveToFile = true, $deleteOriginal = true, $quality = 100) {
        // Valid request?
        if (isset($this->availableConv [$to])) {
            // Load the image
            $res = $this->load($fileName);

            // Get file extension
            $ext = substr($fileName, strrpos($fileName, '.') + 1);

            // Dynamic functions again
            $function = 'image' . trim(strtolower($to));

            // Change the filename
            $fileName = str_replace($ext, $to, $fileName);

            // Set the quality
            if (!is_int($this->availableConv [$to])) {
                $quality = null;
            } elseif ($quality > $this->availableConv [$to]) {
                $quality = $this->availableConv [$to];
            } elseif ($quality < 0) {
                $quality = 0;
            }

            // Output the file to server?
            if ($saveToFile === true) {
                // The acual save function
                if (false === $function($res, $fileName, $quality)) {
                    throw new Exception("Could not save file '" . $fileName . "'.");
                }

                // Delete the original file from the server?
                if ($deleteOriginal === true) {
                    unlink(str_replace($to, $ext, $fileName));
                }

                // Clear the resource
                imagedestroy($res);
            } else {
                // Delete the original file from the server?
                if ($deleteOriginal === true) {
                    unlink(str_replace($to, $ext, $fileName));
                }

                // Return the resource
                return $res;
            }
        } else {
            // Invalid request
            throw new Exception("Invalid image type '" . $to . "'.");
        }
    }

    /**
     * Crops an image from point 1 to point 2
     * 
     * @example 
     * # crop the image from (10,30) to (20,60)
     * $im = $this->image->crop($resource,10,30,20,60);
     * 
     * @param resource $resource
     * @param int $x1
     * @param int $y1
     * @param int $x2
     * @param int $y2
     * @return resource
     */
    public function crop($resource, $x1, $y1, $x2, $y2) {
        // Figure out width and height of cropped section
        $w = abs(intval($x2) - intval($x1));
        $h = abs(intval($y2) - intval($y1));

        // Create empty holder
        $im = $this->canvas($w, $h);

        // This is important; treats inverted cropping (bottom right to top left)
        if ($x1 > $x2) {
            $aux = $x1;
            $x1 = $x2;
            $x2 = $aux;
        }
        if ($y1 > $y2) {
            $aux = $y1;
            $y1 = $y2;
            $y2 = $aux;
        }

        // Do the cropping
        $res = imagecopyresized($im, $resource, 0, 0, $x1, $y1, $w, $h, $w, $h);

        // Return the result
        if ($res !== false) {
            return $im;
        } else {
            throw new Exception("Imagecopyresized failed.");
        }
    }

    /**
     * Displays given image; $filename or $resource
     * 
     * @example 
     * # Send a local image to the browser
     * $this->image->display('testImage.jpeg');
     * # Or display an image resource
     * $this->image->display($resource);
     * # Set the displayed image's type; default = png
     * $this->image->display($resource,'jpeg');
     * # Display it as 'foo.bar'
     * $this->image->display($resource,null,'foo.bar');
     * 
     * @param string|resource $fileNameOrResource
     * @param string $resourceType
     * @param string $displayName
     * @return boolean
     */
    public function display($fileNameOrResource, $TypeOfResource = 'png', $displayName = null) {
        // Shall we commence?
        if (isset($fileNameOrResource)) {
            if (is_string($fileNameOrResource)) {
                // This is a file, then
                $r = $this->load($fileNameOrResource, true);

                // Set the type and resource
                $type = $r ['t'];
                $resource = $r ['r'];

                // Set the display name
                if (is_null($displayName)) {
                    $fileNameOrResource = str_replace(array(DS, WS), DS, $fileNameOrResource);
                    $displayName = substr($fileNameOrResource, strrpos($fileNameOrResource, DS) + 1);
                }
            } elseif (is_resource($fileNameOrResource)) {
                // Set the type and resource
                $type = !is_null($TypeOfResource) ? trim(strtolower($TypeOfResource)) : 'png';

                // Deal with invalid image types
                if (!isset($this->availableConv [$type]))
                    $type = 'png';

                // Set the resource
                $resource = $fileNameOrResource;

                // Set the display name
                if (is_null($displayName)) {
                    $displayName = implode('-', octoms::$oms_url_segments) . '.' . $type;
                }
            } else {
                throw new Exception("The first argument must be either a String or a Resource.");
            }

            // Set the header accordingly
            header('Content-type: image/' . $type);
            header(sprintf('Content-Disposition: inline; filename="%s"', $displayName));

            // Output the image
            call_user_func('image' . $type, $resource);

            // Destroy the image
            imagedestroy($resource);
        } else {
            throw new Exception("You must specify a file name or resource to display.");
        }
    }

    /**
     * This an advanced multifilter and rotator
     * 
     * @example 
     * # Apply 3 filters, rotate the image 5 deg to the left
     * $this->image->filter($resource,'negate,colorize-200-10-10-0,emboss','5');
     * 
     * @param resource $resource
     * @param string $filters - apply one or more of the following filters
     * <ul>
     * <li>negate</li>
     * <li>grayscale</li>
     * <li>brightness-{level}</li>
     * <li>contrast-{level}</li>
     * <li>colorize-{red}-{green}-{blue}-{alpha? 0-255}</li>
     * <li>edgedetect</li>
     * <li>emboss</li>
     * <li>gaussian_blur</li>
     * <li>selective_blur</li>
     * <li>mean_removal</li>
     * <li>smooth-{level}</li>
     * </ul>
     * Example: 'negate,colorize-200-10-10-0,emboss'
     * @param string $rotAng
     * <ul>
     * <li>'-3'    -> minus 3 degrees (image rotates clockwise)</li>
     * <li>'+5'    -> plus 5 degrees (image rotates anticlockwise)</li>
     * <li>'10'    -> plus 10 degrees (equivalent to +10)</li>
     * </ul>
     * @param string $rotBgCol - hexa color
     * <ul>
     * <li>'#ffffff'    -> this means white</li>
     * <li>'#000'        -> this means black; #! short version also accepted</li>
     * </ul>
     * @param string $rotBgAlpha - background transparency 0-127
     * @throws Exception
     * @return resource
     */
    public function filter($resource, $filters = '', $rotAng = '-45', $rotBkgCol = null, $rotBkgAlpha = 0) {
        // Are there any filters?
        if ($filters != '' && $filters != null) {
            // Apply the filters
            foreach ((array) explode(',', $filters) as $filter) {
                // The filter must be a string
                if (!is_string($filter))
                    continue;

                // Do we recognize the filter?
                if (false === strpos($filter, '-')) {
                    // Get the filter name
                    $filter = 'IMG_FILTER_' . strtoupper($filter);

                    // The first argument is the resource
                    $parts = array($resource, constant($filter));
                } else {
                    // Get the filter name and components
                    $parts = explode('-', $filter);
                    $parts[0] = constant($filter = 'IMG_FILTER_' . strtoupper($parts[0]));

                    // The first argument is the resource
                    array_unshift($parts, $resource);
                }

                // Get the filter by calling a constant. #!
                if (defined($filter)) {
                    // Filter the image
                    if (false === call_user_func_array('imagefilter', $parts)) {
                        throw new Exception("Could not apply image filter '{$filter}'.");
                    } else {
                        // All done; repopulate the resource
                        $resource = $parts[0];
                    }
                } else {
                    // Invalid filter
                    throw new Exception("Image filter '{$filter}' does not exist.");
                }
            }
        } # End of multifilter
        // Should we rotate this image?
        if (!empty($rotAng) && !is_null($rotAng)) {
            $resource = $this->rotate($resource, floatval(str_replace('+', '', $rotAng)), $rotBkgCol, $rotBkgAlpha);
        } # End of rotation
        // All done
        return $resource;
    }

    /**
     * Image information
     * 
     * @example 
     * // Get the info on this $resource
     * {image}->info($resource);
     * // What if we saved it as jpeg?
     * {image}->info($resource,'jpeg');
     * 
     * @param resource $resource
     * @param string $type - save type: png/jpeg/gif
     * @return array <ul>
     * <li>width - int, width in pixels</li>
     * <li>height - int, heigh in pixels</li>
     * <li>truecolor - boolean, wether the image is truecolor</li>
     * <li>size - int, the approximate image size in bytes</li>
     * </ul>
     */
    public function info($resource, $type = 'png') {
        // Set the information array
        $info = array(
            'width' => imagesx($resource),
            'height' => imagesy($resource),
            'truecolor' => imageistruecolor($resource)
        );

        // Get the type
        $type = strtolower($type);
        if (!in_array($type, array_keys($this->availableConv)))
            $type = 'png';

        // Output it locally
        ob_start();
        call_user_func('image' . $type, $resource);
        $image = ob_get_clean();

        // Set the size
        $info['size'] = strlen($image);

        // Clean some of the memory
        unset($image);

        // Return the information
        return $info;
    }

    /**
     * Advanced Image Loader
     * 
     * @uses Used by display(), convert()
     * @example 
     * # Get the full information on the image
     * # $resource = array('r'=>$resource,'w'=>200,'h'=>300,'t'=>jpeg);
     * #'r' - image resource
     * #'w' - image width (in pixels)
     * #'h' - image height (in pixels)
     * #'t' - image type
     * $resource = $this->image->load('testImage.jpeg',true);
     * # Or just load it as a resource
     * $resource = $this->image->load('testImage.jpeg');
     * 
     * @throws Exception
     * @param resource|string $fileName
     * @param boolean $advanced
     * @return array
     */
    public function load($fileName, $advanced = false) {
        // Stop if the GD extension is not load
        if (!in_array('gd', get_loaded_extensions())) {
            throw new Exception("You must enable the GD extension in order to perform image manipulations.");
        }

        if (isset($fileName)) {
            // Format the filename
            $fileName = str_replace(array('/', '\\'), DS, $fileName);

            // Get the info
            if (false === $arr = getimagesize($fileName)) {
                throw new Exception("No such file or directory '" . $fileName . "'.");
            }

            // Set the basic array
            $r['w'] = $arr [0];
            $r['h'] = $arr [1];
            $r['t'] = str_replace('image/', '', $arr ['mime']);

            // Is this image valid?
            if (isset($this->availableConv [$r['t']])) {
                // Load the image
                $r ['r'] = call_user_func('imagecreatefrom' . $r ['t'], $fileName);

                // Is it all ok?
                if (gettype($r['r']) != 'resource') {
                    throw new Exception("Could not create image resource from file '" . $fileName . "'.");
                } else {
                    // Do we need to know more?
                    if ($advanced === true) {
                        // Return the large array
                        return $r;
                    } else {
                        // Return the resource
                        return $r['r'];
                    }
                }
            } else {
                throw new Exception("File type '" . $r['t'] . "' not allowed.");
            }
        } else {
            // Not enough params
            throw new Exception("You must specify an image file to load.");
        }
    }

    /**
     * Change image opacity
     * 
     * @example 
     * // Reduce the opacity of an image to 50%
     * {image}->opacity($resource,127);
     * 
     * @param resource $resource
     * @param int $opacity - 0 = transparent, 255 = opaque
     * @throws Exception
     * @return resource $resource
     * 
     */
    public function opacity($resource, $opacity = 255) {
        // Get the width and height of the resource
        $width = imagesx($resource);
        $height = imagesy($resource);

        // Set the opacity limits
        $opacity = intval($opacity);

        // Manage outrageous values
        $opacity = $opacity > 255 ? 255 : ($opacity < 0 ? 0 : $opacity);
        $opacity = ((~$opacity) & 0xff) >> 1;

        for ($y = 0; $y < ($height); $y++) {
            for ($x = 0; $x < ($width); $x++) {
                // Get the color
                $color = imagecolorat($resource, $x, $y);
                $rgba = imagecolorsforindex($resource, $color);

                // Set the new Alpha channel
                $rgba['alpha'] += intval((127 - $rgba['alpha']) * $opacity / 127);

                // Create the new color
                $new = imagecolorallocatealpha($resource, $rgba['red'], $rgba['green'], $rgba['blue'], $rgba['alpha']);

                // Set it in place
                imagesetpixel($resource, $x, $y, $new);
            }
        }

        // All done
        return $resource;
    }

    /**
     * Overlays 2 images (image resources)
     * 
     * @example 
     * // Place the first image (50% opaque) over the second image
     * $res = $this->image->overlay($img1,$img2,'top-center',50);
     * // Place it at 10px X, 20px Y
     * $res = $this->image->overlay($img1,$img2,'10,20',50);
     * 
     * @param resource $overlay
     * @param resource $canvas
     * @param string $overlayPos
     * <ul>
     * <li>'top'</li>
     * <li>'bottom'</li>
     * <li>'left'</li>
     * <li>'center'</li>
     * <li>'right'</li>
     * <li>any CSS valid binomial combination like 'top-center' </li>
     * </ul>
     * <br/> OR <b>'x,y'</b>
     * @param int $overlayAlpha - 0(transparent) to 255(opaque)
     * @throws Exception
     * @return resource
     */
    public function overlay($overlay, $canvas, $overlayPos = 'center', $overlayAlpha = 255) {
        // Set opacity
        if ($overlayAlpha == null) {
            $overlayAlpha = 255;
        } else {
            // Integer, please
            $overlayAlpha = intval($overlayAlpha);

            // Manage outrageous values
            $overlayAlpha = $overlayAlpha > 255 ? 255 : ($overlayAlpha < 0 ? 0 : $overlayAlpha);
        }

        // Valid params, please
        if (isset($overlay) && isset($canvas)) {
            // Canvas width and height
            $c_w = imagesx($canvas);
            $c_h = imagesy($canvas);

            // overlay width and height
            $o_w = imagesx($overlay);
            $o_h = imagesy($overlay);

            // overlay opacity
            $o_o = intval($overlayAlpha);

            // Default overlay
            if (is_null($overlayPos))
                $overlayPos = 'center';

            // Use CSS-like positioning
            if (false === strpos($overlayPos, ',')) {
                // Get the directives
                $exp = array_map('trim', explode('-', $overlayPos));

                // Hardcode these 1-word specs
                if (!isset($exp [1])) {
                    switch ($overlayPos) {
                        case 'left' :
                            $exp [0] = 'top';
                            $exp [1] = 'left';
                            break;
                        case 'center' :
                            $exp [0] = 'center';
                            $exp [1] = 'center';
                            break;
                        case 'right' :
                            $exp [0] = 'top';
                            $exp [1] = 'right';
                            break;
                        case 'top' :
                            $exp [0] = 'top';
                            $exp [1] = 'center';
                            break;
                        case 'bottom' :
                            $exp [0] = 'bottom';
                            $exp [1] = 'center';
                            break;
                    }
                }

                // Correct the word order
                if (!in_array($exp [0], array('top', 'center', 'bottom'))) {
                    // Switch the words
                    $aux = $exp [0];
                    $exp [0] = $exp [1];
                    $exp [1] = $aux;
                    unset($aux);
                }

                // Set position arrays
                $yPosArr = array('top' => 0, 'center' => 0.5, 'bottom' => 1);
                $xPosArr = array('left' => 0, 'center' => 0.5, 'right' => 1);

                // Figure out location of overlay
                if (isset($xPosArr [$exp [1]]) && isset($yPosArr [$exp [0]])) {
                    $cx = ($c_w - $o_w) * floatval($xPosArr [$exp [1]]);
                    $cy = ($c_h - $o_h) * floatval($yPosArr [$exp [0]]);
                } else {
                    // Bad Specs
                    throw new Exception("Your position specifications are bad ({$overlayPos}).");
                }
            }
            // Use exact positions
            else {
                list($cx, $cy) = explode(',', $overlayPos);
            }

            // Set the transparency
            if ($o_o < 255) {
                $overlay = $this->opacity($overlay, $o_o);
            }

            // Overlay the pics
            if (false === $r = imagecopy($canvas, $overlay, $cx, $cy, 0, 0, $o_w, $o_h)) {
                throw new Exception("Could not apply watermark.");
            }

            // All done
            return $canvas;
        } else {
            // Resources were not specified
            throw new Exception("You must specify the 2 images to overlap.");
        }
    }

    /**
     * Perspective transormation
     * 
     * @example 
     * // Give the image some perspective
     * {image}->perspective($image,'0,0','80,20','80,80','0,100');
     * 
     * @param resource $resource
     * @param string $p1 - 'x,y' values
     * @param string $p2
     * @param string $p3
     * @param string $p4
     * @throws Exception
     * @return resource
     */
    public function perspective($resource, $p1 = null, $p2 = null, $p3 = null, $p4 = null) {
        // Get the width and height
        $width = imagesx($resource);
        $height = imagesy($resource);

        // Verify the resource
        if (!$width || !$height) {
            throw new Exception("Please provide a valid resource.");
        }

        // Left, top
        if (false !== strpos($p1, ',')) {
            list($x1, $y1) = explode(',', $p1);
        } else {
            if (!is_null($p1)) {
                $x1 = $p1;
                $y1 = $p1;
            } else {
                $x1 = 0;
                $y1 = 0;
            }
        }

        // Right, top
        if (false !== strpos($p2, ',')) {
            list($x2, $y2) = explode(',', $p2);
        } else {
            if (!is_null($p2)) {
                $x2 = $p2;
                $y2 = $p2;
            } else {
                $x2 = $width;
                $y2 = 0;
            }
        }

        // Right, bottom
        if (false !== strpos($p3, ',')) {
            list($x3, $y3) = explode(',', $p3);
        } else {
            if (!is_null($p3)) {
                $x3 = $p3;
                $y3 = $p3;
            } else {
                $x3 = $width;
                $y3 = $height;
            }
        }

        // Left, bottom
        if (false !== strpos($p4, ',')) {
            list($x4, $y4) = explode(',', $p4);
        } else {
            if (!is_null($p4)) {
                $x4 = $p4;
                $y4 = $p4;
            } else {
                $x4 = 0;
                $y4 = $height;
            }
        }

        // Create a new canvas
        $canvas = $this->canvas($width, $height);

        // Perform the replacements
        for ($x = 0; $x < $width; $x++) {
            for ($y = 0; $y < $height; $y++) {
                // Get the color
                list($dst_x, $dst_y) = $this->_corPix($x1, $y1, $x2, $y2, $x3, $y3, $x4, $y4, $x, $y, $width, $height);
                $color = imagecolorat($resource, $x, $y);
                imagesetpixel($canvas, $dst_x, $dst_y, $color);
            }
        }

        // Destroy the resource
        imagedestroy($resource);

        // All done
        return $canvas;
    }

    /**
     * This function resizes given image and returns it as a resource
     *  
     * @example 
     * # The picture is scaled to width 300px and height 400px
     * $res = $this->image->resize($img,'300','400')
     * # The picture is scaled to 60%
     * $res = $this->image->resize($img,'60');
     * # The picture is scaled so that the final width will be 400px
     * $res = $this->image->resize($img,'width=400');
     * # The picture is scaled so that the final height will be 200px
     * $res = $this->image->resize($img,'height=200');
     * 
     * @param resource $resource
     * @param string|int $width
     * @param string|int $height
     * @param OR string $scale
     * @param OR string $toWidth
     * @param OR string $toHeight
     * @throws Exception
     * @param resource $mode
     */
    public function resize($resource, $params = null) {
        // Get the function arguments
        $args = func_get_args();

        // Get our image's sizes
        $resourceW = imagesx($resource);
        $resourceH = imagesy($resource);

        // Width and Height specified?
        if (count($args) == 3) {
            // We use integer widths and heights
            $width = intval($args [1]);
            $height = intval($args [2]);
        } elseif (count($args) == 2) {
            // Scale?
            if (strpos($args [1], '=') === false) {
                // Get the scale
                $scale = intval($args [1]);

                // Set new dimensions
                $width = $resourceW * $scale / 100;
                $height = $resourceH * $scale / 100;
            } else {
                // Interpret the argument
                $exp = explode('=', $args [1]);

                // resize to width?
                if (trim($exp [0]) == 'width') {
                    $width = intval(trim($exp [1]));
                    $height = intval($width * $resourceH / $resourceW);
                } // resize to height?
                elseif (trim($exp [0]) == 'height') {
                    $height = intval(trim($exp [1]));
                    $width = intval($height * $resourceW / $resourceH);
                }
            }
        }

        // Create the holder
        $holder = $this->canvas($width, $height);

        // Perform some magic
        if (false === $r = imagecopyresized($holder, $resource, 0, 0, 0, 0, $width, $height, $resourceW, $resourceH)) {
            // Something went wrong
            throw new Exception("Could not perform imagecopyresize.");
        }

        // It's all good
        return $holder;
    }

    /**
     * Rotate an image preserving transparency and setting an alpha-layer enabled background
     * 
     * @example
     * // Rotate an image to the right by 30 degrees
     * {image}->rotate($imageResource,30);
     * // Rotate with a grey background (#ccc)
     * {image}->rotate($imageResource,30,'#ccc');
     * // Also make the background 50% transparent (0 = transparent, 255 = opaque)
     * {image}->rotate($imageResource,30,'#ccc',127);
     * 
     * @param resource type GD $resource
     * @param string $angle
     * @param string $bkgColor - Hexa background color (ex. #ccc)
     * @param int $bkgAlpha - Background transparency (0-255)
     * @return resource type GD
     */
    public function rotate($resource, $angle, $bkgColor = null, $bkgAlpha = 0) {
        // Use radians
        $angle = $angle + 180;
        $angle = deg2rad($angle);

        // Set this as truecolor
        $resource = $this->truecolor($resource);

        // Get the image's width and height
        $width = imagesx($resource);
        $height = imagesy($resource);

        // Calculate the image center
        $center_x = floor($width / 2);
        $center_y = floor($height / 2);

        // Sine and cosine values
        $cosangle = cos($angle);
        $sinangle = sin($angle);

        // Set the corners
        $min_x = 0;
        $max_x = 0;
        $min_y = 0;
        $max_y = 0;
        foreach (
        array(
            array(0, 0),
            array($width, 0),
            array($width, $height),
            array(0, $height)
        )
        as $value
        ) {
            $t0 = ($value[0] - $center_x) * $cosangle + ($value[1] - $center_y) * $sinangle;
            $t1 = ($value[1] - $center_y) * $cosangle - ($value[0] - $center_x) * $sinangle;
            if ($t0 < $min_x)
                $min_x = $t0;
            if ($t0 > $max_x)
                $max_x = $t0;
            if ($t1 < $min_y)
                $min_y = $t1;
            if ($t1 > $max_y)
                $max_y = $t1;
        }

        // Set the rotation canvas width and height
        $canvas_width = round($max_x - $min_x);
        $canvas_height = round($max_y - $min_y);

        // Create the image
        $canvas = $this->canvas($canvas_width, $canvas_height);

        // Prepare the background color information
        if (!is_null($bkgColor)) {
            // Use the custom background
            $rgb = $this->_hexToRgb($bkgColor);
            $alpha = intval($bkgAlpha);
            $alpha = $alpha < 0 ? 0 : ($alpha > 255 ? 255 : $alpha);
            $alpha = ((~((int) $alpha)) & 0xff) >> 1;
        } else {
            // Use a transparent background
            $rgb = array('r' => 255, 'g' => '255', 'b' => 255);
            $alpha = 127;
        }

        //Reset center to center of our image
        $newcenter_x = ($canvas_width) / 2;
        $newcenter_y = ($canvas_height) / 2;

        for ($y = 0; $y < ($canvas_height); $y++) {
            for ($x = 0; $x < ($canvas_width); $x++) {
                // Rotate the pixels, one by one
                $old_x = round((($newcenter_x - $x) * $cosangle + ($newcenter_y - $y) * $sinangle)) + $center_x;
                $old_y = round((($newcenter_y - $y) * $cosangle - ($newcenter_x - $x) * $sinangle)) + $center_y;
                if ($old_x >= 0 && $old_x < $width && $old_y >= 0 && $old_y < $height) {
                    // Move a pixel
                    $color = imagecolorat($resource, $old_x, $old_y);
                } else {
                    // Set the background
                    $color = imagecolorallocatealpha($canvas, $rgb['r'], $rgb['g'], $rgb['b'], $alpha);
                }
                // Add the pixel
                imagesetpixel($canvas, $x, $y, $color);
            }
        }

        // All done
        return $canvas;
    }

    /**
     * Save an image resource under given filename
     * 
     * @example 
     * # Save the image resource locally as a jpeg file named "testImage.jpg"
     * $this->image->save($resource,'jpeg','testImage.jpg');
     * # Save the jpeg image at 80% quality; default = 100
     * $this->image->save($resource,'jpeg','img.jpg',80);
     * 
     * @param resource $resource
     * @param string $type
     * @param string $fileName
     * @param string|int $quality
     * @return boolean
     */
    public function save($resource, $type, $fileName, $quality = 100) {
        // Are all arguments in place?
        if (isset($resource) && isset($type) && isset($fileName)) {
            if (isset($this->availableConv [strtolower($type)])) {
                // Set the function name
                $function = 'image' . trim(strtolower($type));

                // Set the quality
                if (!is_int($this->availableConv [$type])) {
                    $quality = null;
                } elseif ($quality > $this->availableConv [$type]) {
                    $quality = $this->availableConv [$type];
                } elseif ($quality < 0) {
                    $quality = 0;
                }

                // Save to file
                if (false === $res = $function($resource, $fileName, $quality)) {
                    throw new Exception("Could not save the image '" . $fileName . "'.");
                }
            } else {
                // Unsupported image type
                throw new Exception("Unsupported image type '" . $type . "'.");
            }
        } else {
            // Not enough arguments
            throw new Exception("Not enough arguments.");
        }
    }

    /**
     * This function stacks up an undetermined number of images
     *
     * @example 
     * # Notice this method can stack up an unlimited number of images
     * $img = $this->image->stack(null,$img1,$img2,$img3,$img4);
     * 
     * @param strimg $mode
     * <ul>
     * <li>'vertical' - the function stacks up images vertically</li>
     * <li>'horizontal' - the function stacks up images horizontally</li>
     * </ul>
     * @param resource $img1
     * @param resource $img2
     * @param resource ...
     * @param resource $img_n
     * @throws Exception
     * @return resource
     */
    public function stack($mode = 'vertical', $img1, $img2) {
        // This function handles an undetermined number of arguments/images
        $args = func_get_args();
        $no_imgs = count($args) - 1;

        // Insufficient arguments?
        if ($no_imgs < 2) {
            throw new Exception("You must specify at least 2 images to stack.");
        }

        // Rewrite the default mode
        if (!is_string($args [0])) {
            $mode = 'vertical';
        } elseif ($args [0] != 'vertical' && $args [0] != 'horizontal') {
            // Invalid mode
            throw new Exception("Invalid stack mode. 'vertical' and 'horizontal' allowed.");
        }

        // Process all images
        for ($i = 1; $i < $no_imgs; $i++) {
            // Image1
            $dw = imagesx($args [$i]);
            $dh = imagesy($args [$i]);

            // Image2
            $sw = imagesx($args [$i + 1]);
            $sh = imagesy($args [$i + 1]);

            if ($mode == 'vertical') {
                // Create the holder
                $holder = $this->canvas($dw > $sw ? $dw : $sw, $dh + $sh);

                // Copy first image onto holder
                if (false === $r = imagecopymerge($holder, $args [$i], 0, 0, 0, 0, $dw, $dh, 100)) {
                    throw new Exception("Imagecopymerge failed.");
                }

                // Copy second image onto holder
                if (false === $r = imagecopymerge($holder, $args [$i + 1], 0, $dh, 0, 0, $sw, $sh, 100)) {
                    throw new Exception("Imagecopymerge failed.");
                }
            } elseif ($mode == 'horizontal') {
                // Create the holder
                $holder = $this->canvas($dw + $sw, $dh > $sh ? $dh : $sh);

                // Copy first image onto holder
                if (false === $r = imagecopymerge($holder, $args [$i], 0, 0, 0, 0, $dw, $dh, 100)) {
                    throw new Exception("Imagecopymerge failed.");
                }

                // Copy second image onto holder
                if (false === $r = imagecopymerge($holder, $args [$i + 1], $dw, 0, 0, 0, $sw, $sh, 100)) {
                    throw new Exception("Imagecopymerge failed.");
                }
            }
            // Good, now replace the second image with the holder
            $args [$i + 1] = $holder;

            // Free-up memory
            unset($holder);
        }

        // Return the last holder
        return $args [$i];
    }

    /**
     * Creates an image resource from given text
     * 
     * @example 
     * # An image of 'hello, world!' written in black, 20px high, using GD font 1
     * $img = $this->image->text('hello, world!','#000000','20',1);
     * # An image of 'hello, world!' written in black at default height of font 'testfont.gdf'
     * $img = $this->image->text('hello, world!','#000000',null,'testfont.gdf');
     * # An image of 'hello, world!' written in default (black) at default height of default font
     * $img = $this->image->text('hello, world!');
     * 
     * @param string $text
     * @param string $color
     * @param string $size
     * @throws Exception
     * @return resource
     */
    public function text($text, $color = null, $size = null, $font = null) {
        // Stop if the GD extension is not load
        if (!in_array('gd', get_loaded_extensions())) {
            throw new Exception("You must enable the GD extension in order to perform image manipulations.");
        }

        // Analyze the string
        $text = trim($text);
        $textLength = strlen($text);

        // Set the color
        if ($color == null || $color == '') {
            $color = $this->defaulttextColor;
        }

        // Custom font?
        if ($font != null) {
            if (strpos($font, '.') !== false) {
                $font = imageloadfont($font);
                if ($font === false) {
                    $font = $this->defaultFont;
                }
            } else {
                $font = intval($font);
            }
        } else {
            $font = $this->defaultFont;
        }

        // Set image parameters
        $width = $textLength * imagefontwidth($font);
        $height = imagefontheight($font);

        // Sizes in pixels
        if ($size == null || $size == '') {
            $size = $height;
        } else {
            $size = intval($size);
        }

        // Create empty drawing
        $textImage = $this->canvas($width, $height);

        // Get the RGB Array
        $c = $this->_hexToRgb($color);
        $b = $this->_hexToRgb($this->defaulttextBg [0]);

        // Allocate transparent white; this automatically loads as background
        $bg = imagecolorallocate($textImage, $b ['r'], $b ['g'], $b ['b']);
        $bg = imagecolortransparent($textImage, $bg);

        // Set the color
        $color = imagecolorallocate($textImage, $c ['r'], $c ['g'], $c ['b']);

        // Create the image
        if (false === $res = imagestring($textImage, $font, 0, 0, $text, $color)) {
            throw new Exception("Could not create an image string.");
        }

        // Let's scale it
        $textImage = $this->resize($textImage, 'height=' . $size);

        // All done!
        return $textImage;
    }

    /**
     * Tile an image
     * 
     * @example 
     * // Tile an image in a 400by400 canvas
     * {image}->tile($image,400);
     * // Tile an image in a 400by800 canvas
     * {image}->tile($image,400,800);
     * 
     * @param resource $resource - the tile
     * @param int $width - canvas width
     * @param int $height - canvas height
     * @throws Exception
     * @return resource
     */
    public function tile($resource = null, $width = null, $height = null) {
        // Verify the resource
        if (is_null($resource) || false === imagesx($resource)) {
            throw new Exception("You must provide a valid image resource to tile.");
        }

        // Format the width and height
        if (is_null($width)) {
            throw new Exception("You must provide a width.");
        } else {
            $width = intval($width);
        }
        if (is_null($height)) {
            $height = $width;
        } else {
            $height = intval($height);
        }

        // Create a 200x200 image
        if (false === $canvas = $this->canvas($width, $height)) {
            throw new Exception("Try using a smaller image or increasing the PHP memory limit.");
        }

        // Set alpha blending to true
        imagealphablending($canvas, true);

        // Set the tile
        if (false === imagesettile($canvas, $resource)) {
            throw new Exception("Could not set the tile.");
        }

        // Make the image repeat
        if (false === imagefilledrectangle($canvas, 0, 0, $width, $height, IMG_COLOR_TILED)) {
            throw new Exception("Imagefilledrectangle failed.");
        }

        // All done
        return $canvas;
    }

    /**
     * Convert an image resource from truecolor (24bit) to 8bit
     * 
     * // 8bit to 24bit
     * {image}->truecolor($resource);
     * // 24bit to 8bit
     * {image}->truecolor($resource,false);
     * 
     * @param resource $resource
     * @param boolean $truecolor
     * @return resource
     */
    public function truecolor($resource, $truecolor = true) {
        // Get the image's sizes
        $sizeX = imagesx($resource);
        $sizeY = imagesy($resource);

        // Convert to truecolor
        if ($truecolor) {
            // Create a blank canvas
            $new = $this->canvas($sizeX, $sizeY);

            // Save the resource
            if (false === imagecopyresampled($new, $resource, 0, 0, 0, 0, $sizeX, $sizeY, $sizeX, $sizeY)) {
                throw new Exception("Could not convert the image to " . ($truecolor ? '24bit' : '8bit') . ".");
            }

            // Free up some memory
            imagedestroy($resource);

            // Return the resource
            return $new;
        }
        // Revert to 8-bit image
        else {
            // The image is already a palette type
            if (!imageistruecolor($resource)) {
                return $resource;
            }

            // Try to convert to palette
            if (false === imagetruecolortopalette($resource, false, 256)) {
                throw new Exception("Could not convert image to 8bit.");
            } else {
                return $resource;
            }
        }
    }
}
    
/* End Of File <image.inc> */