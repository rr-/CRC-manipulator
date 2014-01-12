using System;
using System.Windows;
using System.Windows.Media.Animation;

namespace crcmanip_gui
{
	public static class ControlAnimationExtensionMethods
	{
		public static void FadeIn(this UIElement targetControl)
		{
			DoubleAnimation fadeInAnimation = new DoubleAnimation(0, 1, new Duration(TimeSpan.FromSeconds(0.5)));
			Storyboard.SetTarget(fadeInAnimation, targetControl);
			Storyboard.SetTargetProperty(fadeInAnimation, new PropertyPath(UIElement.OpacityProperty));
			Storyboard sb = new Storyboard();
			sb.Children.Add(fadeInAnimation);
			sb.Begin();
		}

		public static void FadeOut(this UIElement targetControl)
		{
			DoubleAnimation fadeInAnimation = new DoubleAnimation(1, 0, new Duration(TimeSpan.FromSeconds(0.3)));
			Storyboard.SetTarget(fadeInAnimation, targetControl);
			Storyboard.SetTargetProperty(fadeInAnimation, new PropertyPath(UIElement.OpacityProperty));
			Storyboard sb = new Storyboard();
			sb.Children.Add(fadeInAnimation);
			sb.Begin();
		}
	}
}
