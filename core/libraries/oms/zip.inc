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
    
class zip_cl {

    /**
     * Class constructor
     * 
     * @return zip_cl
     */
    public function __construct() {
        if (!extension_loaded('zip')) {
            throw new Exception("The PHP ZIP extension is not loaded.", 1);
        }
    }

    /**
     * Extract a zip file to the specified directory
     * 
     * @example 
     * // Extract a ZIP locally in a directory named 'file'
     * $this->zip->extract('/path/to/file.zip');
     * // Extract a ZIP to a folder named 'file' in the 'destination' directory
     * $this->zip->extract('/path/to/file.zip','/destination');
     * // Extract a ZIP to a 'destination' but not in an enclosed folder
     * $this->zip->extract('/path/to/file.zip','/destination',false);
     * 
     * 
     * @param string $zipFile
     * @param string $dir
     * @param boolean $enclosed - true by default
     * @return boolean true on success, false on failure
     * @throws Exception
     */
    public function extract($zipFile = null, $dir = null, $enclosed = true) {
        // The zip file is mandatory
        if (is_null($zipFile) || !file_exists($zipFile)) {
            throw new Exception("Zip file not found ({$zipFile}).");
        }

        // Clean the path
        $zipFile = str_replace(array('/', '\\'), DS, realpath($zipFile));

        // Get the zip's name
        $zipName = preg_replace('%^[^.]+\\' . DS . '([^.]+).zip$%i', '$1', $zipFile);

        // Get an instance of the ZIP class
        $zip = new ZipArchive();

        // Try to open the ZIP file
        if (true !== $res = @$zip->open($zipFile)) {
            throw new Exception($this->_error($res), $res);
        }

        // Set the destination directory
        if (is_null($dir)) {
            $dir = substr($zipFile, 0, strrpos($zipFile, DS)) . ($enclosed ? DS . $zipName : '');
        } else {
            // Clean the name
            $dir = rtrim(str_replace(array('/', '\\'), DS, realpath($dir)), DS) . ($enclosed ? DS . $zipName : '');
        }

        // Create the directory
        if (!is_dir($dir)) {
            // Make the directory recursive; sort of dangerous
            if (@!mkdir($dir, 0755, true)) {
                throw new Exception("Could not create directory ({$dir}).");
            }
        }
        // The directory already exists; stop
        else {
            throw new Exception("Trying to extract over an existing directory({$dir}).");
        }

        // Extract the file
        if (true !== $res = @$zip->extractTo($dir)) {
            throw new Exception($this->_error($res), $res);
        }

        // All done
        $zip->close();
        return true;
    }

    /**
     * Compress a zip archive
     * 
     * @example 
     * // Compress the "foo" directory as "bar.zip"
     * $this->zip->compress("foo");
     * // Compress the "foo" directory as "bar.zip"
     * $this->zip->compress("foo","bar");
     * // Compress the "foo" directory as "bar.zip" but enclosing the "foo" directory in "bar.zip"
     * $this->zip->compress("foo","bar",true);
     * // Replace the content of the "foo/baz.txt" file
     * $this->zip->compress("foo",null,null,array("foo/baz.txt"=>"New content"));
     * // Use a custom method to replace the content of a file; 
     * // This method expects 1 parameter (string); it must return a string (binary-safe)
     * $this->zip->compress("foo",null,null,array("foo/baz.txt"=>array($object,'method')));
     * 
     * 
     * @param string $dir - Directory to archive
     * @param string $zipName - Name of the zip file
     * @param boolean $enclosed - Wether or not to enclose the dir in the zip (false by default)
     * @param $replaceFiles - An array of files to replace
     */
    public function compress($dir = null, $zipName = null, $enclosed = false, $replaceFiles = array()) {
        // We need the dir
        if (is_null($dir)) {
            throw new Exception("Please supply a valid directory path.");
        }

        // Sanitize the directory
        $dir = str_replace(array('/', '\\'), DS, realpath($dir));

        // Verify the directory exists
        if (!is_dir($dir)) {
            throw new Exception("Directory " . $dir . " does not exist.");
        }

        // Save the directory's name
        $exp = explode(DS, $dir);
        $dirName = array_pop($exp);

        // Can we create a zip near the directory?
        if (!is_dir(implode(DS, $exp))) {
            throw new Exception("Cannot archive the root.");
        }

        // Create a new ZIP instance
        $zip = new ZipArchive();

        // Create the new zip
        $zip->open(implode(DS, $exp) . DS . (is_null($zipName) ? $dirName : $zipName) . '.zip', ZIPARCHIVE::OVERWRITE);

        // Use an enclosed folder?
        if ($enclosed) {
            $zip->addEmptyDir($dirName);
        }

        // Format the "replaceFiles"
        $rf = array();
        foreach ($replaceFiles as $k => $v) {
            $rf[str_replace(array('/', '\\'), DS, $k)] = $v;
        }

        // Iterate through the Directory
        $dirs = array($dir);
        while (count($dirs)) {
            $dir = current($dirs);
            $zipDir = trim(($enclosed ? $dirName . DS : '') . trim(preg_replace('%^' . implode('\\' . DS, $exp) . '\\' . DS . $dirName . '%i', '', $dir), DS), DS);
            if ($zipDir != '') {
                $zip->addEmptyDir($zipDir);
            }
            $dh = opendir($dir);
            while (false !== $file = readdir($dh)) {
                if ($file != '.' && $file != '..') {
                    if (is_file($dir . DS . $file)) {
                        $localName = ($zipDir != '' ? $zipDir . DS : '') . $file;
                        // Replace the file with a new content?
                        if (isset($rf[$localName])) {
                            $newContent = $rf[$localName];
                            // Use a custom method
                            if (is_array($newContent) && count($newContent) == 2) {
                                if (method_exists($newContent[0], $newContent[1])) {
                                    if (false !== $fh = @fopen($dir . DS . $file, 'r')) {
                                        if (false !== $oldContent = fread($fh, filesize($dir . DS . $file))) {
                                            $newContent = @call_user_func($newContent, $oldContent);
                                        }
                                        fclose($fh);
                                    }
                                }
                            }

                            // Only if the custom method above worked or if the content was supplied as string
                            if (!is_array($newContent)) {
                                $zip->addFromString($localName, $newContent);
                            }
                            // The custom method failed; fallback to the original
                            else {
                                $zip->addFile($dir . DS . $file, $localName);
                            }
                        }
                        // Or just add it from the original?
                        else {
                            $zip->addFile($dir . DS . $file, $localName);
                        }
                    } elseif (is_dir($dir . DS . $file)) {
                        $dirs[] = $dir . DS . $file;
                    }
                }
            }
            closedir($dh);
            array_shift($dirs);
        }
        $zip->close();
    }

    /*
     * Interpret errors
     * 
     * @param string $error
     * @return string - Error details
     */
    private function _error($error = null) {
        switch ($error) {
            case ZipArchive::ER_MULTIDISK :
                return 'Multidisk ZIP Archives not supported';

            case ZipArchive::ER_RENAME :
                return 'Failed to rename the temporary file for ZIP';

            case ZipArchive::ER_CLOSE :
                return 'Failed to close the ZIP Archive';

            case ZipArchive::ER_SEEK :
                return 'Failure while seeking the ZIP Archive';

            case ZipArchive::ER_READ :
                return 'Failure while reading the ZIP Archive';

            case ZipArchive::ER_WRITE :
                return 'Failure while writing the ZIP Archive';

            case ZipArchive::ER_CRC :
                return 'CRC failure within the ZIP Archive';

            case ZipArchive::ER_ZIPCLOSED :
                return 'ZIP Archive already closed';

            case ZipArchive::ER_NOENT :
                return 'No such file within the ZIP Archive';

            case ZipArchive::ER_EXISTS :
                return 'ZIP Archive already exists';

            case ZipArchive::ER_OPEN :
                return 'Can not open ZIP Archive';

            case ZipArchive::ER_TMPOPEN :
                return 'Failure creating temporary ZIP Archive';

            case ZipArchive::ER_ZLIB :
                return 'ZLib Problem';

            case ZipArchive::ER_MEMORY :
                return 'Memory allocation problem while working on a ZIP Archive';

            case ZipArchive::ER_CHANGED :
                return 'ZIP Entry has been changed';

            case ZipArchive::ER_COMPNOTSUPP :
                return 'Compression method not supported within ZLib';

            case ZipArchive::ER_EOF :
                return 'Premature EOF within ZIP Archive';

            case ZipArchive::ER_INVAL :
                return 'Invalid argument for ZLIB';

            case ZipArchive::ER_NOZIP :
                return 'Given file is no zip archive';

            case ZipArchive::ER_INTERNAL :
                return 'Internal error while working on a ZIP Archive';

            case ZipArchive::ER_INCONS :
                return 'Inconsistent ZIP archive';

            case ZipArchive::ER_REMOVE :
                return 'Can not remove ZIP Archive';

            case ZipArchive::ER_DELETED :
                return 'ZIP Entry has been deleted';
            default :
                return 'Unknown error within ZIP Archive';
        }
    }
}
    
/* End Of File <zip.inc> */