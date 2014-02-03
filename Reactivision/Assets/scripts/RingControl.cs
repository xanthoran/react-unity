using UnityEngine;
using System.Collections;

public class RingControl : MonoBehaviour {

	private float startingAlpha;
	private float startingScale;

	public float downAlphaAmount = 0.5f;
	public float alphaCycleTime = 2.0f;

	public float downScaleAmount = 0.5f;
	public float scaleCycleTime = 2.0f;


	// Use this for initialization
	void Start () {
	//	startingAlpha = renderer.material.color.a;
	//	startingScale = transform.localScale.x;


		iTween.RotateAdd(gameObject, iTween.Hash("z", 25, "easeType", "easeInOutBack", "loopType", "pingPong", "delay", 1.0,"time",4.0));

		//AlphaDown ();
		//ScaleDown ();



		//RotateBy(

	}

	void ScaleDown(){
		//alpha off
		ValueTo (startingScale, downScaleAmount, scaleCycleTime, 0.0f, "easeInOutCubic", "ScaleUp", "ChangeScale");
	}

	void ScaleUp(){
		//alpha off
		ValueTo (transform.localScale.x, startingScale, scaleCycleTime, 0.0f, "easeInOutCubic", "ScaleDown", "ChangeScale");
	}

	void AlphaDown(){
		//alpha off
		ValueTo (startingAlpha, downAlphaAmount, alphaCycleTime, 0.0f, "easeInOutCubic", "AlphaUp", "ChangeAlpha");
	}

	void AlphaUp(){
		//alpha on
		ValueTo (renderer.material.color.a, startingAlpha, alphaCycleTime, 0.0f, "easeInOutCubic", "AlphaDown", "ChangeAlpha");
	}

	
	// Update is called once per frame
	void Update () {
		
	}

	public void ChangeAlpha(float newValue){
		Color newColor = this.renderer.material.color;
		newColor.a = newValue;
		this.renderer.material.color = newColor;

	}

	public void ChangeScale(float newValue){

		Vector3 newScale = new Vector3 (newValue, newValue, newValue);

		this.transform.localScale = newScale;


		
	}


	public void ValueTo (float start, float end, float time, float delay, string easetype, string callback, string update){
		Hashtable animationParams = new Hashtable ();
		animationParams.Add ("from", start);
		animationParams.Add ("to", end);
		animationParams.Add ("time", time);
		animationParams.Add ("delay", delay);
		animationParams.Add ("easetype", easetype);
		
		if (callback.Length > 0) {
			//Callback when animation is over
			animationParams.Add ("oncomplete", callback);
		}
		
		if (update.Length > 0) {
			animationParams.Add ("onupdate", update);
			
		}
		
		iTween.ValueTo (this.gameObject, animationParams);
	}
	
	
	
	public void RotateBy (Vector3 end, float time, string easetype, string callback){
		Hashtable animationParams = new Hashtable ();
		animationParams.Add ("amount", end);
		animationParams.Add ("time", time);
		animationParams.Add ("easetype", easetype);
		
		if(callback.Length > 0){
			//Callback when animation is over
			animationParams.Add ("oncomplete", callback);
		}
		
		animationParams.Add ("isLocal", true);
		
		iTween.RotateBy (this.gameObject, animationParams);
	}
	
	public void RotateTo (Vector3 end, float time, string easetype, string callback){
		Hashtable animationParams = new Hashtable ();
		animationParams.Add ("rotation", end);
		animationParams.Add ("time", time);
		animationParams.Add ("easetype", easetype);
		
		if(callback.Length > 0){
			//Callback when animation is over
			animationParams.Add ("oncomplete", callback);
		}
		
		animationParams.Add ("isLocal", true);
		
		iTween.RotateTo (this.gameObject, animationParams);
	}
	
	public void MoveTo (Vector3 end, float time, string easetype, string callback){
		Hashtable animationParams = new Hashtable ();
		animationParams.Add ("position", end);
		animationParams.Add ("time", time);
		animationParams.Add ("easetype", easetype);
		
		if(callback.Length > 0){
			//Callback when animation is over
			animationParams.Add ("oncomplete", callback);
		}
		
		animationParams.Add ("isLocal", true);
		
		iTween.MoveTo (this.gameObject, animationParams);
	}















}



