extends Node

@onready var root = get_tree().get_first_node_in_group("root")
@onready var ui = root.ui
@export var websocket_url = "ws://127.0.0.1:8080"

enum NetState {
	IDLE,
	AUTHENTICATING,
	AUTHENTICATING_WAIT_RESPONSE,
	PLAYING
}

var net_state := NetState.IDLE;
var socket = WebSocketPeer.new()
var player_nickname = null

func connect_to_server(nickname: String):
	var err = socket.connect_to_url(websocket_url)
	if err != OK:
		ui.info_label.text = "Can't connect to %s" % [websocket_url]
		set_process(false)
		return
	self.player_nickname = nickname
	net_state = NetState.AUTHENTICATING
	set_process(true)
	ui.info_label.text = "Connecting to %s" % [websocket_url]

func _process(_delta):
	socket.poll()
	var state = socket.get_ready_state()
	if state == WebSocketPeer.STATE_OPEN:
		if net_state == NetState.AUTHENTICATING:
			ui.info_label.text = "Authenticating"
			socket.send_text(JSON.stringify({
				"nickname": self.player_nickname
			}))
			net_state = NetState.AUTHENTICATING_WAIT_RESPONSE
		elif net_state == NetState.AUTHENTICATING_WAIT_RESPONSE:
			if socket.get_available_packet_count() == 0:
				return
			var packet = socket.get_packet()
			if !socket.was_string_packet():
				ui.info_label.text = "Unexpected data from server"
				socket.close()
				return
			var packet_text = packet.get_string_from_utf8()
			var data = JSON.parse_string(packet_text)
			if data["result"] == false:
				ui.info_label.text = "Server declined authentication: %s" % [data["message"]]
				socket.close()
				return
			ui.info_label.text = ""
		elif net_state == NetState.PLAYING:
			pass

	elif state == WebSocketPeer.STATE_CLOSING:
		pass
	
	elif state == WebSocketPeer.STATE_CLOSED:
		var code = socket.get_close_code()
		ui.info_label.text = "WebSocket closed with code: %d" % [code]
		set_process(false)
