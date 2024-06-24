<?php
	if(isset($_GET['ref']))
		$ref = $_GET['ref'];

	if(isset($_GET['host']))
	{
		$host = $_GET['host'];
		if(empty($ref))
			$ref = "http://$host";
	}

	if(empty($ref) && isset($_SERVER['HTTP_REFERER']))
		$ref = $_SERVER['HTTP_REFERER'];

	if(empty($host) && isset($_SERVER['HTTP_HOST']))
		$host = $_SERVER['HTTP_HOST'];

	if(isset($_GET['url']))
	{
		$arg = "";
		foreach($_GET as $h=>$v)
		{
			if($h != "url" && $h != "ref" && $h != "host")
				$arg = "$arg&$h=$v";
		}

		$c = curl_init();
		curl_setopt($c, CURLOPT_URL, $_GET['url'] . $arg);
		curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($c, CURLOPT_BINARYTRANSFER, true);
		curl_setopt($c, CURLOPT_HEADER, false);
		curl_setopt($c, CURLOPT_FOLLOWLOCATION, true);
		curl_setopt($c, CURLOPT_REFERER, $ref);

		$response = curl_exec($c);
		header("Content-Type: " . curl_getinfo($c, CURLINFO_CONTENT_TYPE));

		print($response);
		curl_close($c);
	}
	else
	{
		if(function_exists('curl_version'))
			echo curl_version()."\n";
		else
			echo "CURL extension is not installed on this server !!!\n";

		echo $_SERVER['REQUEST_METHOD']."\n";
		foreach($_SERVER as $h=>$v)
		{
			if(ereg('HTTP_(.+)',$h,$hp))
			{
				if(isset($host) && $h == "HTTP_HOST")
					$v = $host;

				if(isset($ref) && $h == "HTTP_REFERER")
					$v = $ref;

				echo "<li>$h = $v</li>\n";
			}
		}
	}
?>
