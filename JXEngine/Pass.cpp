#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Pass.h"
#include "FrameBuffer.h"
#include "Scene.h"
#include "Material.h"

//void Pass::Active()
//{
//	if(frame.)
//}

void Pass::Active()
{
}

void Pass::Draw()
{
	if (mat.use_count() < 1)
		scene->Draw();
	else
		scene->Draw(mat);

}

void Pass::BindOutput()
{
	if (frame.use_count() < 1)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, frame->FBO());
}

void Pass::BindOutput(Output f)
{
	glBindFramebuffer(GL_FRAMEBUFFER, f->FBO());
}

void Pass::BindOutput(unsigned int fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Pass::UpdateInput(Input scene)
{
	this->scene = scene;
}

Pass::Output Pass::GetOutput()
{
	return frame;
}

void Pass::UpdateOutput(Output frame)
{
	this->frame = frame;
}

Pass::GlobalMat Pass::GetMat()
{
	return mat;
}

void Pass::UpdateGlobalMat(GlobalMat mat)
{
	this->mat = mat;
}



