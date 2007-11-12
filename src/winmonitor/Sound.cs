using System;
using System.Runtime.InteropServices;

namespace XRefresh
{
	// http://www.codeguru.com/csharp/csharp/cs_graphics/sound/article.php/c6143/
	public class Sound
	{
		// provide access to PlaySound() in the Multi-media system API through MMSYSTEM.dll.
		[DllImport("winmm")]
		public static extern bool PlaySound(string pszSound, Int32 hFile, UInt32 fdwSound);

		// overloaded version of PlaySound. Allows passing a null to the hFile argument
		[DllImport("winmm")]
		public static extern bool PlaySound(string pszSound, string hFile, UInt32 fdwSound);

		// flag values for SoundFlags argument on PlaySound
		private const UInt32 SND_SYNC        = 0x0000;      // play synchronously (default)
		private const UInt32 SND_ASYNC       = 0x0001;      // play asynchronously
		private const UInt32 SND_NODEFAULT   = 0x0002;      // silence (!default)if sound not found
		private const UInt32 SND_MEMORY      = 0x0004;      // pszSound points to a memory file
		private const UInt32 SND_LOOP        = 0x0008;      // loop the sound until next sndPlaySound
		private const UInt32 SND_NOSTOP      = 0x0010;      // don't stop any currently playing sound
		private const UInt32 SND_NOWAIT      = 0x00002000;  // don't wait if the driver is busy
		private const UInt32 SND_ALIAS       = 0x00010000;  // name is a Registry alias
		private const UInt32 SND_ALIAS_ID    = 0x00110000;  // alias is a predefined ID
		private const UInt32 SND_FILENAME    = 0x00020000;  // name is file name
		private const UInt32 SND_RESOURCE    = 0x00040004;  // name is resource name or atom
		private const UInt32 SND_PURGE       = 0x0040;      // purge non-static events for task
		private const UInt32 SND_APPLICATION = 0x0080;      // look for application-specific association

		//-----------------------------------------------------------------
		public void Play(string wfname)
		{
			PlaySound(wfname, null, SND_ASYNC);
		}

		//-----------------------------------------------------------------
		public void Stop()
		{
			PlaySound(null, null, SND_PURGE);
		}
	}
}
