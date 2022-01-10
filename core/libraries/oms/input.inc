<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
class input_cl {
    /**
     * Set the default upload location relative to the ROOT
     * The constructor will change this to ROOT.DS.{}
     */
    public $uploadDir = 'upload';

    /**
     * Class constructor
     */
    public function __construct() {
        // Fix quotes gpc settings; one time only
        if (!isset($this->fix_gpc)) {
            if (get_magic_quotes_gpc()) {
                $process = array(&$_GET, &$_POST, &$_COOKIE, &$_REQUEST);
                while (false !== list($key, $val) = each($process)) {
                    foreach ($val as $k => $v) {
                        unset($process[$key][$k]);
                        if (is_array($v)) {
                            $process[$key][stripslashes($k)] = $v;
                            $process[] = &$process[$key][stripslashes($k)];
                        } else {
                            $process[$key][stripslashes($k)] = stripslashes($v);
                        }
                    }
                }
                unset($process);

                // Performed quotes gpc fix 
                $this->fix_gpc = true;
            } else {
                // Quotes gpc fix was not needed
                $this->fix_gpc = false;
            }
        }

        // Set the upload directory
        $this->uploadWeb = (WEBROOT != WS ? WEBROOT . WS : WS) . $this->uploadDir;
        $this->uploadDir = ROOT . DS . $this->uploadDir;
    }

    /**
     * Get super globals
     * 
     * @example # Return $_POST["k"]
     * $this->input->sg('post','k');
     * # Return $_GET['a'] and $_GET['b']
     * $this->input->sg('get',array('a','b'));
     * # Return $_ENV
     * $this->input->sg('env');
     * 
     * @param string $superGlobal - (get, post, server or env)
     * @param string|array $val
     * @return string|array|null <br/>
     * String - if $var is a string and {superGlobal}[$var] exists<br/>
     * Array - if $var is an array and some of {superGlobal}[$key] exist<br/>
     * null - if the key was not set
     */
    public function sg($superGlobal = null, $val = null) {
        // Format the SuperGlobal's name
        if (empty($superGlobal)) {
            return null;
        } else {
            $sg = trim(strtoupper($superGlobal));
            switch ($sg) {
                case 'GET': $sg = isset($_GET) ? $_GET : array();
                    break;
                case 'POST': $sg = isset($_POST) ? $_POST : array();
                    break;
                case 'SERVER': $sg = isset($_SERVER) ? $_SERVER : array();
                    break;
                case 'ENV': $sg = isset($_ENV) ? $_ENV : array();
                    break;
                default: $sg = array();
            }
        }

        // Nothing to return
        if (count($sg) == 0) {
            return null;
        }

        // Get all the keys
        if (empty($val)) {
            return $sg;
        } else {
            if (is_array($val)) {
                $arr = array();
                foreach ($val as $v) {
                    if (isset($sg[$v])) {
                        $arr[$v] = $sg[$v];
                    } else {
                        $arr[$v] = null;
                    }
                }
                return $arr;
            } elseif (is_string($val) || is_numeric($val)) {
                if (isset($sg[$val])) {
                    return $sg[$val];
                }
            } else {
                return null;
            }
        }
    }

    /**
     * Return _GET variables without unsetting this superglobal
     * 
     * @example # Return $_GET["key"]
     * $this->input->get("key");
     * # Return $_GET["k1"] and $_GET["k2"]
     * $this->input->get(array("k1","k2"));
     * # Return true if $_GET is set, false otherwise
     * $this->input->get();
     * # Ruturn the keys of $_GET
     * $this->input->get(true);
     * 
     * @param string|array $var - $_GET key(s) to return
     * @param array $clean - clean filters to apply
     * @return string|array|null <br/>
     * String - if $var is a string and $_GET[$var] exists<br/>
     * Array - if $var is an array and some of $_GET[$key] exist<br/>
     * null - if the key was not set
     */
    public function get($var = null, $clean = array()) {
        if ($var === true)
            return isset($_GET) ? array_keys($_GET) : null;
        if ($var === null)
            return (isset($_GET) && count($_GET) > 0) ? true : false;
        $r = func_get_args();
        if (!isset($r[2]))
            $var = $this->sg('get', $var);

        if (is_array($var)) {
            $result = array();
            foreach ($var as $k => $v) {
                if (!is_null($v) && !is_null($v = $this->get($v, $clean, true))) {
                    $result[$k] = $v;
                }
            }
            return $result;
        } else {
            // Maybe just one filter?
            if (is_string($clean))
                $clean = array($clean);

            // Apply the filters
            if (is_array($clean) && count($clean) > 0) {
                foreach ($clean as $filter) {
                    if (method_exists($this, 'clean_' . $filter)) {
                        $var = call_user_func_array(array($this, 'clean_' . $filter), array($var));
                    }
                }
            }
        }
        return urldecode($var);
    }

    /**
     * Delete a key
     * 
     * @example 
     * # Remove the $_POST['username']
     * $this->input->del('post','username');
     * # Remove the $_POST['username'] and $_POST['password']
     * $this->input->del('post',array('username','password'));
     * 
     * @param string $sg - SuperGlobal (GET, POST, SERVER or ENV)
     * @param string $key
     * @return input_cl
     */
    public function del($sg = null, $key = null) {
        // Must be a proper superglobal
        if (!in_array(strtoupper($sg), array('GET', 'POST', 'SERVER', 'ENV'))) {
            return $this;
        }

        // Unset the value
        if (is_array($key)) {
            foreach ($key as $k) {
                @eval('unset($_' . strtoupper($sg) . '[$k]);');
            }
        } elseif (!is_null($key)) {
            @eval('unset($_' . strtoupper($sg) . '[$key]);');
        }

        // All done
        return $this;
    }

    /**
     * Return _POST variables without unsetting this superglobal
     * 
     * @example # Return $_POST["key"]
     * $this->input->post("key");
     * # Return $_POST["k1"] and $_POST["k2"]
     * $this->input->post(array("k1","k2"));
     * # Return true if $_POST is set, false otherwise
     * $this->input->post();
     * # Return the $_POST keys
     * $this->input->post(true);
     * 
     * @param string|array $var - $_POST key(s) to return
     * @param array $clean - clean filters to apply
     * @return string|array|null <br/>
     * String - if $var is a string and $_POST[$var] exists<br/>
     * Array - if $var is an array and some of $_POST[$key] exist<br/>
     * null - if the key was not set
     */
    public function post($var = null, $clean = array()) {
        if ($var === true)
            return isset($_POST) ? array_keys($_POST) : null;
        if ($var === null)
            return (isset($_POST) && count($_POST) > 0) ? true : false;
        $r = func_get_args();
        if (!isset($r[2]))
            $var = $this->sg('post', $var);

        if (is_array($var)) {
            $result = array();
            foreach ($var as $k => $v) {
                if (!is_null($v) && !is_null($v = $this->post($v, $clean, true))) {
                    $result[$k] = $v;
                }
            }
            return $result;
        } else {
            // Maybe just one filter?
            if (is_string($clean))
                $clean = array($clean);

            // Apply the filters
            if (is_array($clean) && count($clean) > 0) {
                foreach ($clean as $filter) {
                    if (method_exists($this, 'clean_' . $filter)) {
                        $var = call_user_func_array(array($this, 'clean_' . $filter), array($var));
                    }
                }
            }
        }

        return $var;
    }

    /**
     * Check wether a file is pending a save through POST
     * 
     * @example
     * # Get all files ($_FILES)
     * $this->input->file();
     * # Get the file info for form input named "file1"
     * $this->input->file('file1'); 
     * 
     * @param string $formInputName
     * @return array|null <br/>
     * Returns null if the requested form input was not passed.
     */
    public function file($formInputName = null) {
        if (isset($_FILES) && is_array($_FILES)) {
            if (!empty($formInputName)) {
                return isset($_FILES[$formInputName]) ? $_FILES[$formInputName] : null;
            } else {
                return (count($_FILES) > 0) ? $_FILES : null;
            }
        } else {
            return null;
        }
    }

    /**
     * Upload a file
     * 
     * @example 
     * # Upload a file from the input named 'file1'
     * # ex. <form..><input name="file1" type="file"/>
     * # and save it directly to the root, without changing its name
     * $this->input->upload('file1',ROOT);
     * # Same as above but save the file as "doc.png"
     * $this->input->upload('file1',ROOT,'doc.png');
     * # Same as above but only allow PNG files
     * $this->input->upload('file1',ROOT,'doc.png','image/png');
     * # Same as above but only allow PNG and GIF files
     * $this->input->upload('file1',ROOT,'doc.png',array('image/png','image/gif'));
     * # Same as above but limit file size to 2000B
     * $this->input->upload('file1',ROOT,'doc.png',array('image/png','image/gif'),2000);
     * # Same as above but limit file size to 500-2000B
     * $this->input->upload('file1',ROOT,'doc.png',array('image/png','image/gif'),array(500,2000));
     * # Allow files with the PNG extension
     * $this->input->upload('file1',ROOT,null,null,null,'png');
     * # Allow files with the PNG and GIF extensions
     * $this->input->upload('file1',ROOT,null,null,null,array('png','gif');
     * 
     * @param string $formInputName
     * @param string $folder
     * @param string $desiredName
     * @param string|array $allowedTypes
     * @param string|array $allowedSizes
     * @param string|array $allowedExt
     * @param boolean $replaceFile
     * @return boolean true on success, throws Exception otherwise
     * @throws Exception
     */
    public function upload($formInputName = null, $folder = null, $desiredName = null, $allowedTypes = null, $allowedSizes = null, $allowedExt = null, $replaceFile = false) {
        // The form input name is mandatory
        if (empty($formInputName))
            throw new Exception('Form input not specified.', 0);

        // Verify the input
        if (!isset($_FILES[$formInputName]))
            throw new Exception('No file specified for input "' . $formInputName . '".', 1);

        // Save the file
        $f = $_FILES[$formInputName];

        // Format the folder name
        $folder = trim(str_replace(array(WS, ' '), array(DS, '_'), preg_replace('%[^a-zA-Z0-9_/\\\\\s]%i', '', $folder)));
        if (strlen($folder) == 0) {
            throw new Exception("Invalid destination folder.", 2);
        }

        // The default desired name is the user's local filename
        if (empty($desiredName))
            $desiredName = $f['name'];

        // Do not allow dangerous file names
        if (in_array($desiredName, array('.htaccess', 'php.ini', 'httpd.conf'))) {
            throw new Exception('File not allowed.', 3);
        }

        // Verify the type
        if (is_array($allowedTypes)) {
            if (!in_array($f['type'], $allowedTypes)) {
                throw new Exception('File type (' . $f['type'] . ') not allowed.', 4);
            }
        } elseif (is_string($allowedTypes)) {
            if (strtolower(trim($f['type'])) != strtolower(trim($allowedTypes))) {
                throw new Exception('File type (' . $f['type'] . ') not allowed.', 4);
            }
        }

        // Verify the size
        if (is_array($allowedSizes)) {
            if ($f['size'] < intval($allowedSizes[0]) || $f['size'] > intval($allowedSizes[1])) {
                throw new Exception('File size (' . $f['size'] . ') not allowed.', 5);
            }
        } elseif (is_string($allowedSizes) || is_numeric($allowedSizes)) {
            if ($f['size'] > intval($allowedSizes)) {
                throw new Exception('File size (' . $f['size'] . ') not allowed.', 5);
            }
        }

        // Verify the extension
        $ext = strpos($f['name'], '.') === false ? '' : (substr($f['name'], strrpos($f['name'], '.') + 1));
        if (is_array($allowedExt)) {
            if (!in_array($ext, $allowedExt)) {
                throw new Exception('File extension "' . $ext . '" not allowed.', 6);
            }
        } elseif (is_string($allowedExt)) {
            if (strtolower(trim($ext)) != strtolower(trim($allowedExt))) {
                throw new Exception('File extension "' . $ext . '" not allowed.', 6);
            }
        }

        // Verify that no other errors occured
        if ($f['error'] > 0) {
            // http://php.net/manual/en/features.file-upload.errors.php
            $errors = array(
                'No error.',
                'The uploaded file exceeds the upload_max_filesize directive in php.ini.',
                'The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form.',
                'The uploaded file was only partially uploaded.',
                'No file was uploaded.',
                'An undocumented error occured.',
                'Missing a temporary folder.',
                'Failed to write file to disk.',
                'A PHP extension stopped the file upload.'
            );
            $message = isset($errors[$f['error']]) ? $errors[$f['error']] : 'An unkwnown error occured.';
            throw new Exception('Return code (' . $message . ')', 7);
        }

        // Verify that we are not attempting an overwrite
        if (!$replaceFile && file_exists(ROOT . DS . 'upload' . DS . $folder . DS . $desiredName)) {
            throw new Exception('File already exists', 8);
        }

        // Create the folder if it does not exist
        if (!is_dir($this->uploadDir . DS . $folder)) {
            if (!mkdir($this->uploadDir . DS . $folder, 0644, true)) {
                throw new Exception('Could not create directory "' . $folder . '"', 9);
            }
        }

        // Upload the file!
        if (!move_uploaded_file($f['tmp_name'], $this->uploadDir . DS . $folder . DS . $desiredName)) {
            throw new Exception('Could not move the file "' . $f['name'] . '" to "' . $folder . '".', 10);
        }

        // All done
        return true;
    }

    /**
     * File list - Get files from te upload folder
     * 
     * @example 
     * // Get the files from ROOT/UPLOAD_FOLDER/foo
     * $this->input->file_list('foo');
     * // Get the files from ROOT/UPLOAD_FOLDER
     * $this->input->file_list();
     * 
     * @param string $folder
     * @return array|null - array of array('name'=>string,'size'=>int,'url'=>string) or null on failure
     */
    public function file_list($folder = null) {
        // The directory
        $dir = rtrim($this->uploadDir . DS . $folder, DS);

        // Get all the files in the directory
        if (is_dir($dir)) {
            $filesFound = array();
            if (false !== $files = scandir($dir)) {
                foreach ($files as $file) {
                    if (is_file($dir . DS . $file)) {
                        $filesFound[] = array(
                            'name' => $file,
                            'size' => filesize($dir . DS . $file),
                            'url' => $this->uploadWeb . WS . str_replace(DS, WS, $folder) . WS . urlencode($file)
                        );
                    }
                }
            }
            // Return the findings
            return $filesFound;
        }

        // Nothing found
        return null;
    }

    /**
     * Get a JSON input from a _post variable
     * 
     * @example 
     * # Decode the JSON in $_POST['x']
     * $data = $this->input->json('x');
     * # Decode the JSON from a local variable $var
     * $data = $this->input->json($var,true);
     * 
     * 
     * @param string $data
     * @param boolean $local
     * @return mixed - Array or string on success, False if there is no 
     * post data or Null if the data cannot be decoded
     */
    public function json($data, $local = false) {
        // Get the json
        if ($local == false && false === ($data = $this->post($data))) {
            return $data;
        }

        // Try to decode the data
        if (null === ($data = json_decode($data))) {
            return $data;
        }

        // Load the recursion helper
        if (!isset(octoms::$oms_helpers['recursion_ch'])) {
            require_once CORE_HELP . DS . 'recursion_ch' . EXT;
            octoms::$oms_helpers['recursion_ch'] = true;
        }

        // Use arrays
        return object_to_array($data);
    }

    /**
     * Remove html tags
     * 
     * @example 
     * # Remove all HTML tags except 'a','img' and 'code'
     * $this->input->clean_tags($input);
     * # Remove all HTML tags
     * $this->input->clean_tags($input,true);
     * # Remove all HTML tags except the 'p'
     * $this->input->clean_tags($input,array('p'));
     * 
     * @param string $input
     * @param boolean|array $tags
     * @return string
     */
    public function clean_tags($input = '', $tags = null) {
        if (is_null($tags)) {
            return strip_tags($input, '<' . implode('><', array('a', 'img', 'code')) . '>');
        } elseif (is_array($tags)) {
            return strip_tags($input, '<' . implode('><', $tags) . '>');
        } else {
            return strip_tags($input);
        }
    }

    /**
     * Remove line feeds and extra whitespaces
     * 
     * @example 
     * $clean = $this->input->clean_whitespaces($input);
     * 
     * @param string $input
     * @return string
     */
    public function clean_whitespaces($input = '') {
        // Strip line feeds and excessive tabs, spaces and &nbsp;
        return preg_replace('%[\r\n]+|<br[^>]*/>|(&nbsp;){2,}|[\s]{2,}%i', ' ', $input);
    }

    /**
     * Remove images but keep alts, if possible
     * 
     * @example 
     * $clean = $this->input->clean_images($input);
     * 
     * @param string $input
     * @return string
     */
    public function clean_images($input = '') {
        // Preserve the alternative text and anchor
        $input = preg_replace('%(<a[^>]*>)(<img[^>]+alt=")([^"]*)("[^>]*>)(<\/a>)%i', '$1$3$5', $input);

        // Preserve the alternative text even if it is not anchored
        $input = preg_replace('%(<img[^>]+alt=")([^"]*)("[^>]*>)%i', '$2', $input);

        // Remove all of the other images
        return preg_replace('%<img[^>]*>%i', '', $input);
    }

    /**
     * Remove scripts, links, styles and html comments
     * 
     * @example 
     * $clean  = $this->input->clean_scripts($input);
     * 
     * @param string $input
     * @return string
     */
    public function clean_scripts($input = '') {
        return preg_replace('%<script[^>]*>.*?<\/script>|<[\?\%]+[php]?[^>]*[\?\%]?>?|(<link[^>]+rel="[^"]*stylesheet"[^>]*>|style="[^"]*")|<style[^>]*>.*?<\/style>|<!--.*?-->%is', '', $input);
    }

    /**
     * Clean all
     * 
     * @example 
     * # Clean to the bone!
     * $str = $this->input->clean_all($str);
     * 
     * @param string $str
     * @return string
     */
    public function clean_all($str = '') {
        $str = $this->clean_tags($str, true);
        $str = $this->clean_whitespaces($str);
        $str = $this->clean_images($str);
        return $this->clean_scripts($str);
    }

    /**
     * Clean the HTML
     * 
     * @example 
     * # Keep al html tags (like a,div etc.)
     * $this->input->clean_html($input);
     * # Remove all html tags
     * $this->input->clean_html($input,false);
     * # Remove 'random' tags
     * $this->input->clean_html($input,array('random'));
     * # Don't encode quotes
     * $this->input->clean_html($input,null,ENT_NOQUOTES);
     * 
     * @param string $input
     * @param array|boolean $keepTags
     * @param string $quote - ENT_QUOTES | ENT_COMPAT | ENT_NOQUOTES
     * @return string
     */
    public function clean_html($input = '', $keepTags = true, $quote = ENT_QUOTES) {
        // Preserve HTML tags?
        if (is_array($keepTags)) {
            $input = strip_tags($input, '<' . implode('><', $keepTags) . '>');
        } elseif ($keepTags === false) {
            $input = strip_tags($input);
        }

        // Return the html encoded string
        return htmlentities($input, $quote, 'UTF-8');
    }

    /**
     * Clean an article/post/comment
     * 
     * @param string $str
     * @return string
     */
    public function clean_article($str = '') {
        // Paragraphs are NL-separated
        $str = preg_replace('/<([^>]*?)>(.*?)<\1>/', '\2' . "\n", $str);

        // Remove whitespaces
        $str = preg_replace('%(&nbsp;){2,}|[ \t]{2,}%i', ' ', $str);

        // Find and sanitize all <code> blocks
        $str = preg_replace_callback('%(<code[^>]*>)([^\e]*)</code>%i', create_function('$matches', 'return "<code>".(octoms(\'input\')->clean_html($matches[2]))."</code>";'), $str);

        // Remove unwanted tags
        $str = $this->clean_tags($str);

        // Remove images
        $str = $this->clean_images($str);

        // Remove scripts
        $str = $this->clean_scripts($str);

        // Return the result
        return trim($str);
    }

    /**
     * Close open tags
     * 
     * @example 
     * 
     * @param string $html
     * @return string - tags closed
     */
    public function tags_close($html = '') {
        // Find open tags
        preg_match_all('%<(?!meta|img|br|hr|input|link\b)\b([a-z]+)\b%iU', $html, $o);

        // Find closed tags
        preg_match_all('%</([a-z]+)>%iU', $html, $c);

        // All good?
        if (count($o[1]) == count($c[1]))
            return $html;

        // Close the HTML tags
        foreach (array_reverse($o[1]) as $k => $v) {
            if (!in_array($v, $c[1])) {
                $html .= '</' . $v . '>';
            } else {
                unset($c[1][array_search($v, $c[1])]);
            }
        }

        // Return the result
        return $html;
    }

    /**
     * Remove broken tags
     * 
     * @example 
     * 
     * @param string $html
     * @return string
     */
    public function tags_whipe($html = '') {
        return preg_replace('%<[^\b<>]*$%', '', $html);
    }

    /**
     * Create an abstract
     * 
     * @example 
     * 
     * @param string $string
     * @param int $len - number of words
     * @return string
     */
    public function abstr($string, $len = 500) {
        // Clean the whitespaces
        $string = $this->clean_whitespaces($string);

        // Any trimming to be done?
        if (strlen($string) > $len) {
            $pos = ($len - stripos(strrev(substr($string, 0, $len)), ' '));
            $sum = substr($string, 0, $pos - 1);
            $chr = $sum[strlen($sum) - 1];
            if (strpos($chr, '.,!?;:')) {
                $sum = substr($sum, 0, strlen($sum) - 1);
            }
            return $this->tags_close($this->tags_whipe($sum . '&#8230;'));
        } else {
            return $string;
        }
    }
}

/* End Of File <input.inc> */