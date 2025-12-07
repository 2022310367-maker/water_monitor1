<?php

require __DIR__ . '/vendor/autoload.php';

use Kreait\Firebase\Factory;

$factory = (new Factory)
    ->withServiceAccount('firebase_credentials.json')
    ->withDatabaseUri('https://water-monitoring-system-aac9b-default-rtdb.asia-southeast1.firebasedatabase.app/');

$database = $factory->createDatabase();

return $database; 

?>
