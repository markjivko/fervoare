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

if (!function_exists('ago')) {
    /**
     * Time ago
     * 
     * @example 
     * // Show how much time has passed, with the highest u.m.
     * ago(1333361278);
     * 
     * // Show how much time has passed, with all u.m.
     * ago(1333361278,true);
     * 
     * @param string $time - Unix Timestamp
     * @param boolean $rcs - Wether to show recursive time, up to the second
     * @return string - How much time has passed
     */
    function ago($time, $rcs = false) {
        // Store the current time
        $cur_tm = time();

        // Get the time difference
        $dif = $cur_tm - $time;

        // Get the units of measuerement
        $pds = array('second', 'minute', 'hour', 'day', 'week', 'month', 'year', 'decade');
        $lngh = array(1, 60, 3600, 86400, 604800, 2630880, 31570560, 315705600);

        // Calculate the highest u.m.
        for ($v = sizeof($lngh) - 1; ($v >= 0) && (($no = $dif / $lngh[$v]) <= 1); $v--);
        if ($v < 0) {
            $v = 0;
        }

        // Get the time left
        $_tm = $cur_tm - ($dif % $lngh[$v]);

        // Format the number
        $no = floor($no);
        if ($no <> 1)
            $pds[$v] .= 's';
        $x = sprintf("%d %s ", $no, $pds[$v]);

        // Recursive call?
        if (($rcs) && ($v >= 1) && (($cur_tm - $_tm) > 0)) {
            $x .= ago($_tm);
        }

        // All done
        return $x;
    }
}

if (!function_exists('array_depth')) {
    /**
     * Calculate the depth of an array
     * 
     * @example 
     * // Returns 1
     * array_depth(array('foo','bar'));
     * // Returns 2
     * array_depth(array('foo',array('bar','baz')));
     * 
     * @param array $array
     * @return int - array depth
     */
    function array_depth($array) {
        // Not an array? 0 depth
        if (!is_array($array)) {
            return 0;
        }

        // Start with 1
        $max = 1;

        // Use the print_r function for this
        foreach (explode("\n", print_r($array, true)) as $line) {
            $i = (strlen($line) - strlen(ltrim($line))) / 4;
            $max = $i > $max ? $i : $max;
        }

        // Done
        return intval(ceil(($max - 1) / 2) + 1);
    }
}

if (!function_exists('array_flat')) {
    /**
     * Flatten an array to 1 level; unique values;
     * 
     * @example 
     * // Return 'a','b','c'
     * array_flat(array('a',array('b','c')));
     * 
     * @param array $array
     * @return array - flattened array
     */
    function array_flat($array = array()) {
        $r = array();

        foreach ($array as $a) {
            if (!is_array($a)) {
                $r[] = $a;
            } else {
                $r = array_merge($r, array_flat($a));
            }
        }

        return array_unique($r);
    }
}

if (!function_exists('array_is_assoc')) {
    /**
     * Check wether an array is associative
     * 
     * @example 
     * // Check if $foo is associative
     * // $foo=array('a','b','c');
     * array_is_assoc($foo); # false
     * 
     * // $foo=array('a','1'=>'b','2'=>'c');
     * array_is_assoc($foo); # true
     * 
     * // $foo=array('a'=>'b');
     * array_is_assoc($foo); # true
     * 
     * @param array $array
     * @return boolean - True if the array is associative
     */
    function array_is_assoc($array) {
        // Make sure this is an array
        if (!is_array($array)) {
            return false;
        }

        // Check associativity
        return (boolean) count(array_filter(array_keys($array), 'is_string'));
    }
}

if (!function_exists('array_structure')) {
    /**
     * Structure an array according to the serialized rules
     * 
     * @example 
     * # $array = array('a','b','c','d','e');
     * # $rules = array('root',0,1,1,2);
     * # Returns array('a'=>array('b'=>array(3=>'d','c'=>array(4=>'e'))));
     * $result = array_structure($array,$rules);
     * 
     * @param array $array - 1D List of items
     * @param array $rules - 1D Rules of placement; must be in ASC order, 'root' being lowest;
     * Each value in $rules shows the parent key for the corresponding item in $array;
     * Must begin with 'root'.
     * @return multidimensional array
     */
    function array_structure($array, $rules) {
        // Prepare the result
        $result = array();
        foreach ($rules as $key => $value) {
            if ($value === 'root') {
                $result[$key] = $array[$key];
            } elseif (isset($result[$value])) {

                // Prepare the new rules
                $nr = array();
                foreach ($rules as $k => $v) {
                    // Replace the current value with "root"
                    if ($v === $value) {
                        $nr[$k] = 'root';
                    }
                    // And remove old roots
                    elseif (is_numeric($v)) {
                        $nr[$k] = $rules[$k];
                    }
                }
                unset($result[$value]);
                $result[$array[$value]] = array_structure($array, $nr, true);
            }
        }
        return $result;
    }
}

if (!function_exists('array_to_object')) {
    /**
     * Array to object - Transform an array into an object recursively
     * 
     * @example 
     * // Transform array $array to an object
     * $obj = array_to_object($array);
     * 
     * @param array $data
     * @return object|mixed - Returns the input $data unaltered if it
     * is not an array (it could be an integer, a resource etc.)
     */
    function array_to_object($data = null) {
        if (is_array($data) || is_object($data)) {
            $result = new stdClass();
            foreach ($data as $key => $value) {
                $result->$key = array_to_object($value);
            }
            return $result;
        }
        return $data;
    }
}

if (!function_exists('array_to_xml')) {
    class SimpleXMLExtended extends SimpleXMLElement {
        public function addCData($cdata_text) {
            $node = dom_import_simplexml($this);
            $no = $node->ownerDocument;
            $node->appendChild(
                $no->createCDATASection(
                    $cdata_text
                )
            );
        }
    }

    /**
     * Turn an array to an XML string
     * 
     * @example 
     * // Get a simple xml <baz><foo><![CDATA[bar]]></foo></baz>
     * array_to_xml(array('foo'=>'bar'),'baz');
     * // Add some attributes <baz><foo bar="baz" abc="def"><![CDATA[bar]]></foo></baz>
     * array_to_xml(array('foo:bar=baz,abc=def'=>'bar'),'baz');
     * 
     * @param array $array
     * @param string $name
     * @return string|boolean - XML formatted string OR False on failure
     */
    function array_to_xml($array, $name = null, &$xml = null) {
        // Set the top XML
        if ($name != null) {
            $xml = new SimpleXMLExtended(
                '<?xml version="1.0"?><' . $name . '></' . $name . '>',
                LIBXML_NOCDATA
            );
        }

        // We need the XML
        if (is_null($xml)) {
            return false;
        }

        // Parse the array
        foreach ($array as $key => $value) {
            // Get attributes, if they were set
            if (false !== strpos($key, ':')) {
                // Get the attributes
                $attributes = substr($key, strpos($key, ':') + 1);

                // Cleant the key
                $key = substr($key, 0, strpos($key, ':'));

                // Split the attributes
                $attributes = explode(',', $attributes);
            } else {
                $attributes = array();
            }

            // Parse the array
            if (is_array($value)) {
                if (!is_numeric($key)) {
                    $subnode = $xml->addChild($key);
                    array_to_xml($value, null, $subnode);
                } else {
                    $subnode = $xml->addChild('key_' . $key);
                    array_to_xml($value, null, $subnode);
                }
            } else {
                if (!is_numeric($key)) {
                    $subnode = $xml->addChild($key);
                    $subnode->addCData($value);
                } else {
                    $subnode = $xml->addChild('key_' . $key);
                    $subnode->addCData($value);
                }
            }

            // Append the extra data
            if (count($attributes) > 0) {
                foreach ($attributes as $a) {
                    $exp = explode('=', $a);
                    if (!isset($a[1]))
                        $a[1] = '';
                    $subnode->addAttribute($exp[0], $exp[1]);
                }
            }
        }

        // All done
        if ($name != null) {
            return $xml->asXML();
        }
    }
}

if (!function_exists('ini')) {
    /**
     * Verify if an INI value was set to true
     * 
     * @example 
     * // Verify "allow_url_fopen"
     * ini("allow_url_fopen");
     * 
     * @param string $setting - PHP.INI directive
     * @return null if not set, true if activated, false if deactivated, String if other value is provided
     * 
     */
    function ini($setting = null) {
        // We need this!
        if (is_null($setting)) {
            return null;
        } else {
            // The user set this
            if ("" !== $v = ini_get($setting)) {
                if (in_array(strtolower($v), array('1', 'on'))) {
                    return true;
                } elseif (in_array(strtolower($v), array('0', 'off'))) {
                    return false;
                } else {
                    return $v;
                }
            } else {
                // This was not set
                return null;
            }
        }
    }
}

if (!function_exists('object_to_array')) {
    /**
     * Object to array - Transform an object into an array recursively
     * 
     * @example 
     * // Transform object $obj to an array
     * $array = object_to_array($obj);
     * 
     * @param object $data
     * @return array|mixed - Returns the input $data unaltered if it
     * is not an object (it could be an integer, a resource etc.)
     */
    function object_to_array($data = null) {
        if (is_array($data) || is_object($data)) {
            $result = array();
            foreach ($data as $key => $value) {
                $result[$key] = object_to_array($value);
            }
            return $result;
        }
        return $data;
    }
}

if (!function_exists('remove_dir')) {
    /**
     * Completely remove a directory and all files in it
     * 
     * @example 
     * // Remove /uploads/folder
     * remove_dir('/uploads/folder');
     * 
     * @param $dir
     * @return boolean - true on success, false on failure
     */
    function remove_dir($dir = null) {
        if (!is_null($dir) && false !== $dh = opendir($dir)) {
            while (false !== ($file = @readdir($dh))) {
                if (!in_array($file, array('.', '..'))) {
                    @chmod($dir . DS . $file, 0777);
                    if (is_dir($dir . DS . $file)) {
                        remove_dir($dir . DS . $file);
                    } else {
                        unlink($dir . DS . $file);
                    }
                }
            }
            closedir($dh);
            @rmdir($dir);
            return true;
        } else {
            return false;
        }
    }
}

if (!function_exists('typeof')) {
    /**
     * Get the type of a variable
     * 
     * @example 
     * // Returns 'string'
     * typeof("abc");
     * 
     * 
     * @param mixed $var
     * @return string - the variable type
     * 
     */
    function typeof($var = null) {
        return gettype($var);
    }
}
    
/* End Of File <recursion.inc> */