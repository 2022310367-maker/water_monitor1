<?php
header('Content-Type: application/json');

// Load Firebase connection
$database = require __DIR__ . '/firebase_connect.php';

// Read whole database (root)
$reference = $database->getReference('/');
$snapshot = $reference->getSnapshot();
$data = $snapshot->getValue();

// Output RAW JSON
echo json_encode($data, JSON_PRETTY_PRINT);
?>
