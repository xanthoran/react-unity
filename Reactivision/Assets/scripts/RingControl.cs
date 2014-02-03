using UnityEngine;
using System.Collections;



public class RingControl : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
		iTween.RotateBy(gameObject, iTween.Hash("z", .25, "easeType", "easeInOutBack", "loopType", "pingPong", "delay", .4,"space","global"));
	

	}
	
	// Update is called once per frame
	void Update () {
	


	}
}
