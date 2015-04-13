#if defined _time_included
  #endinput
#endif
#define _time_included

//Time Functions
native Time(&hour=0, &minute=0, &second=0);
native Date(&year=0, &month=0, &day=0);

native TimestampCurrent();
native TimestampDetails(seconds1970, &year=0, &month=0, &day=0, &hour=0, &minute=0, &second=0);

/***
<api title="Core Scripting Functions">
	<function name="Time">
		<summary>Get Current time</summary>
		<param name="&amp;hour" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;minute" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;second" default="0" type="n">Variable to store the infomation.</param>
	</function>
	<function name="Date">
		<summary>Get Current Date</summary>
		<param name="&amp;year" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;month" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;day" default="0" type="n">Variable to store the infomation.</param>
	</function>
	<function name="TimestampCurrent">
		<summary>Returns the seconds from 1970. Also known as Unix Time</summary>
		<return>Returns the seconds from 1970</return>
	</function>
	<function name="TimestampDetails">
		<summary>Converts a Unix Timestamp to a time and date.</summary>
		<param name="seconds1970" default="" type="n">Timestamp to use</param>
		<param name="&amp;year" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;month" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;day" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;hour" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;minute" default="0" type="n">Variable to store the infomation.</param>
		<param name="&amp;second" default="0" type="n">Variable to store the infomation.</param>
	</function>
</api>
***/
