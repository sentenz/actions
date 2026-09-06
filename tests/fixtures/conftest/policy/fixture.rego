package main

import rego.v1

deny contains "fixture must be allowed" if {
	input.allowed != true
}
