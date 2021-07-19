<?php if (! defined ( 'OCTOMS' )){header ( 'HTTP/1.1 403' );die ( '{"error":"forbidden"}' );}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

class captcha_l {

    public $RECAPTCHA_API_SERVER = 'http://www.google.com/recaptcha/api';
    public $RECAPTCHA_API_SECURE_SERVER = 'https://www.google.com/recaptcha/api';
    public $RECAPTCHA_VERIFY_SERVER = 'www.google.com';
    public $public_key = '';
    public $private_key = '';
    public $error = null;

    /**
     * Constructor
     */
    public function __construct() {
        // Use the settings
        $this->settings = octoms('settings', OMS_L);
        $this->input = octoms('input', OMS_CL);

        // Set the keys
        $this->public_key = $this->settings->get('general.captcha.public_key');
        $this->private_key = $this->settings->get('general.captcha.private_key');
    }

    /**
     * Initialize the captcha; also verify the user input
     * 
     * @example 
     * // Initialize the captcha and send the response to $this->method
     * {captcha}->init(array($this,'method'));
     * 
     * @param array $function - (object,method_name) a method which is called after the user submits the form<br/>
     * This method accepts 1 parameter, type stdClass with 2 properties:<ul>
     * <li>$r->is_valid - boolean</li>
     * <li>$r->error - the error message</li>
     * </ul>
     * @param string $public_key - Overwrites the public key settings
     * @param string $private_key - Overwrites the private key settings
     * @return captcha_l
     */
    public function init($function = array(), $public_key = null, $private_key = null) {
        // Reset the keys?
        if (!is_null($public_key))
            $this->public_key = $public_key;
        if (!is_null($private_key))
            $this->private_key = $private_key;

        // Any post?
        if ($this->input->post('recaptcha_challenge_field') && $this->input->post('recaptcha_response_field')) {
            // Get the response
            try {
                $r = $this->check_answer(
                        $this->input->post('recaptcha_challenge_field'),
                        $this->input->post('recaptcha_response_field')
                );
                if (!$r->is_valid)
                    $this->error = $r->error;
            } catch (Exception $e) {
                $r = new stdClass();
                $r->is_valid = false;
                $r->error = $e->getMessage();
            }

            // Call the user-defined function
            if (count($function) == 2 && method_exists($function[0], $function[1])) {
                call_user_func($function, $r);
            }
        }

        // Done
        return $this;
    }

    /**
     * Gets the challenge HTML (javascript and non-javascript version).
     * This is called from the browser, and the resulting reCAPTCHA HTML widget
     * is embedded within the HTML form it was called from.
     * @param boolean $use_ssl Should the request be made over ssl? (optional, default is false)
     * @return string - The HTML to be embedded in the user's form.
     */
    public function html($theme = null, $use_ssl = false) {
        if (!is_null($theme) && in_array($theme, array('red', 'white', 'blackglass', 'clean', 'custom'))) {
            $custom = js("var RecaptchaOptions = {theme:'{$theme}'};", false);
        } else {
            $custom = '';
        }
        $server = $use_ssl ? $this->RECAPTCHA_API_SECURE_SERVER : $this->RECAPTCHA_API_SERVER;
        $errorpart = !is_null($this->error) ? ("&amp;error=" . $this->error) : "";

        // Return the string
        return $custom . js(sprintf('%s/challenge?k=%s', $server, $this->public_key . $errorpart));
    }

    /**
     * Calls an HTTP POST function to verify if the user's guess was correct
     * 
     * @param string $challenge
     * @param string $response
     * @param array $extra_params an array of extra variables to post to the server
     * @return ReCaptchaResponse
     */
    protected function check_answer($challenge = '', $response = '', $extra_params = array()) {
        $privkey = $this->private_key;
        $remoteip = $_SERVER['REMOTE_ADDR'];

        //discard spam submissions
        if ($challenge == null || strlen($challenge) == 0 || $response == null || strlen($response) == 0) {
            $recaptcha_response = new stdClass ();
            $recaptcha_response->is_valid = false;
            $recaptcha_response->error = 'incorrect-captcha-sol';
            return $recaptcha_response;
        }

        $response = $this->_post($this->RECAPTCHA_VERIFY_SERVER, "/recaptcha/api/verify", array('privatekey' => $privkey, 'remoteip' => $remoteip, 'challenge' => $challenge, 'response' => $response) + $extra_params);
        $answers = explode("\n", $response [1]);
        $recaptcha_response = new stdClass ();

        if (trim($answers [0]) == 'true') {
            $recaptcha_response->is_valid = true;
        } else {
            $recaptcha_response->is_valid = false;
            $recaptcha_response->error = $answers [1];
        }
        return $recaptcha_response;
    }

    /**
     * Return an english translation of the error
     * 
     * @param string $errCode
     * @return string
     */
    protected function _error($errCode = null) {
        // Return nothing
        if (is_null($errCode))
            $errCode = $this->error;

        // Define the errors
        $errors = array(
            'invalid-site-private-key' => 'We weren\'t able to verify the private key.',
            'invalid-request-cookie' => 'The challenge parameter of the verify script was incorrect.',
            'incorrect-captcha-sol' => 'The CAPTCHA solution was incorrect.',
            'recaptcha-not-reachable' => 'reCAPTCHA server could not be contacted.'
        );

        // Return the translation
        if (in_array($errCode, array_keys($errors))) {
            return $errors[$errCode];
        } else {
            return 'Unexpected error. Please refresh the page and try again.';
        }
    }

    /**
     * Encodes the given data into a query string format
     * 
     * @param string $data - array of string elements to be encoded
     * @return string - encoded request
     */
    protected function _encode($data = null) {
        // The data is mandatory
        if (is_null($data)) {
            return null;
        }

        // Prepare the result
        $result = array();

        // Clean the input
        foreach ($data as $key => $value) {
            $result [] = sprintf("%s=%s", $key, urlencode(stripslashes($value)));
        }

        // Return the result
        return implode("&", $result);
    }

    /**
     * Submits an HTTP POST to a reCAPTCHA server
     * 
     * @param string $host
     * @param string $path
     * @param array $data
     * @param int port
     * @return array response
     */
    protected function _post($host, $path, $data, $port = 80) {
        $req = $this->_encode($data);
        $http_request = "POST $path HTTP/1.0\r\n";
        $http_request .= "Host: $host\r\n";
        $http_request .= "Content-Type: application/x-www-form-urlencoded;\r\n";
        $http_request .= "Content-Length: " . strlen($req) . "\r\n";
        $http_request .= "User-Agent: reCAPTCHA/PHP\r\n";
        $http_request .= "\r\n";
        $http_request .= $req;

        $response = '';
        if (false == ($fs = @fsockopen($host, $port, $errno, $errstr, 10))) {
            throw new Exception("Could not open socket. {$errstr}");
        }
        fwrite($fs, $http_request);

        // Read the file
        while (!feof($fs)) {
            $response .= fgets($fs, 1160);
        }
        fclose($fs);
        $response = explode("\r\n\r\n", $response, 2);

        // Return the response
        return $response;
    }

    /**
     * Perform a 16 bit padding
     * 
     * @param string $val
     */
    protected function _aes_pad($val = '') {
        $numpad = 16 - (strlen($val) % 16);
        return str_pad($val, strlen($val) + $numpad, chr($numpad));
    }

    /**
     * Aes Encrypt
     * 
     * @param string $val
     * @param string $ky
     */
    protected function _aes_encrypt($val = '', $ky = '') {
        if (!function_exists("mcrypt_encrypt")) {
            throw new Exception("To use reCAPTCHA Mailhide, you need to have the mcrypt php module installed.");
        }
        $mode = MCRYPT_MODE_CBC;
        $enc = MCRYPT_RIJNDAEL_128;
        $val = $this->_aes_pad($val);
        return mcrypt_encrypt($enc, $ky, $val, $mode, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
    }
}

/* End Of File <captcha.inc> */