s = Server.default;
s.boot;

(
SynthDef (
	"OneSine",
	{
		|freq = 440|
		Out.ar(0, SinOsc.ar(freq, 0, 0.1)); 	}
).send(s); // save the file and send it to the server s )

z = Synth("OneSine"); //play the node
z.nodeID = 1000;
z.set("freq", 600); //change the freq 

z = Synth("OneSine"); //play the node
z.nodeID = 1001;
z.set("freq", 700); //change the freq 