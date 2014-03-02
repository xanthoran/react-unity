using UnityEngine;
using System.Collections;

using System.Collections.Generic;


public class OSC_Controller : MonoBehaviour {

	// Use this for initialization

	List<object> values;

	void Start () {
	
		OSCHandler.Instance.Init ();

		//OSCHandler.Instance.SendMessageToClient ("HolodeckClient", "address/folder/1", 13);



		values = new List<object> ();
		values.AddRange (new object[]{1.0f,1.0f});


	}
	
	// Update is called once per frame
	void Update () {



		OSCHandler.Instance.SendMessageToClient ("HolodeckClient", "live/arm", values );
	
	}
}
