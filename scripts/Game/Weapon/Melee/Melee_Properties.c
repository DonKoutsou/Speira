
modded class SCR_MeleeWeaponProperties
{
	[Attribute("10", UIWidgets.Slider, "Size of damage dealt by the weapon", "0.0 500.0 1.0", category: "Global")]
	private float m_fDamage;	
	[Attribute("1", UIWidgets.Slider, "Range of the weapon [m]", "1 5 0.5", category: "Global")]
	private float m_fRange;
	[Attribute("1", UIWidgets.Slider, "how long it takes to execute the attack", "0.0 10.0 0.1", category: "Global")]
	private float m_fExecutionTime;

	[Attribute("5", UIWidgets.Slider, "Number of measurements in frame", "1 20 1", category: "Hit detection")]
	private int m_iNumOfMeasurements;
	[Attribute("", UIWidgets.Coords, "List of coords where the collision probes will be placed (related to model)", category: "Hit detection")]
	private ref array<vector> m_aCollisionProbesPos;	
	[Attribute("1", UIWidgets.Slider, "Number of failed probes check that cause the attack will be canceled immediately", "1 10 1", category: "Hit detection")]
	private int m_iNumOfFailedProbesTolerance;
}