extends Node3D

# Getters instead of @onready: Godot calls _ready() on children before their
# parent, so @onready vars here would still be null when Net/UI read them.
var net: Node:
	get: return $Net
var ui: Control:
	get: return $UI
