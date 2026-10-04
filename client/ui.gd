extends Control

@onready var root := get_tree().get_first_node_in_group("root");
@onready var net = root.net
@onready var info_label = $InfoLabel as Label
@onready var nickname_line_edit := $CenterContainer/PanelContainer/HBoxContainer/NicknameLineEdit

func _on_button_pressed() -> void:
	var nickname = nickname_line_edit.text as String
	if nickname.is_empty():
		self.info_label.text = "Invalid nickname"
		return
	net.connect_to_server(nickname)
