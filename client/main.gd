extends Node


@export var websocket_url = "ws://127.0.0.1:8080"


var net_state := 0;
var socket = WebSocketPeer.new()


func _ready():
	var err = socket.connect_to_url(websocket_url)
	if err == OK:
		print("Connecting to %s..." % websocket_url)
		
		# await get_tree().create_timer(2).timeout

		
		# print("> Sending test packet.")
		# socket.send_text("Test packet")
	else:
		push_error("Unable to connect.")
		set_process(false)


func _process(_delta):
	socket.poll()
	
	var state = socket.get_ready_state()
	
	if state == WebSocketPeer.STATE_OPEN:
		print("Connected")
		socket.close()
		# if net_state == 0:
		# 	if socket.get_available_packet_count() > 0:
		# while socket.get_available_packet_count():
		# 	var packet = socket.get_packet()
		# 	if socket.was_string_packet():
		# 		var packet_text = packet.get_string_from_utf8()
		# 		print("< Got text data from server: %s" % packet_text)
		# 	else:
		# 		print("< Got binary data from server: %d bytes" % packet.size())

	elif state == WebSocketPeer.STATE_CLOSING:
		pass
	
	elif state == WebSocketPeer.STATE_CLOSED:
		var code = socket.get_close_code()
		print("WebSocket closed with code: %d. Clean: %s" % [code, code != -1])
		set_process(false)
