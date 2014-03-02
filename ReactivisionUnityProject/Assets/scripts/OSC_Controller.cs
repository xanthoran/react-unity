using UnityEngine;
using System.Collections;

public class OSC_Controller : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
		OSCHandler.Instance.Init ();

		OSCHandler.Instance.SendMessageToClient ("HolodeckClient", "address/folder/1", 13);


	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
